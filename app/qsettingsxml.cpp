#pragma once
#include <QtCore/QtCore>
#include <QtXml/QtXml>

bool readSettingsXml(QIODevice &device, QMap<QString, QVariant> &map);
bool writeSettingsXml(QIODevice &device, const QMap<QString, QVariant> &map);

static const QSettings::Format xmlFormat = QSettings::registerFormat("xml", &readSettingsXml, &writeSettingsXml);
static const QString rootName = "config";

//the following code is all used to save QSettings in xml. reading is accomplished
//through a stream-based xml parser. writing uses a stack-based approach to get
//all leaves of the tree-like QSettings structure right next to each other in the resulting xml file. writing example:
/*	we want: (which is our current result)
<root>
	<category1>
		<entry1 />
		<entry2 />
	</category1>
	<category2 />
</root>

what we don't want:
<root>
	<category1>
		<entry1 />
	</category1>
	<categrory2 />
	<category1>
		<entry2 />
	</category1>
</root>
*/




//helper class for saving QSettings as xml,
//used for both reading & writing.
//children are used only for writing.
//parent used only for reading.
class XmlNode : public QObject
{
public:
    QString tagName, subtext;

    XmlNode(const QString &name, const QString &text = QString(), QObject *parent = 0) :
            QObject(parent),
            tagName(name),
            subtext(text)
    {
    }

    QString fullPath() const
    {
        const XmlNode *cur = this;
        QString path = tagName;

        while((cur = (const XmlNode *) cur->parent()) != 0)
            path.prepend(cur->tagName + "\\");

        return path.mid(rootName.size() + 1); // remove root node & trailing slash
    }
};

bool readSettingsXml(QIODevice &device, QMap<QString, QVariant> &map)
{
    QXmlStreamReader xml(&device);
    XmlNode *curNode = 0;

    while(!xml.atEnd())
    {
        switch(xml.readNext())
        {
            case QXmlStreamReader::StartElement:
                if(curNode != 0)
                    //we're already processing the file if there already is a current node
                    curNode = new XmlNode(xml.name().toString(), QString(), curNode);
                else if(xml.name().toString() == rootName)
                    //no current node? this must be the first one: the root
                    curNode = new XmlNode(rootName);
                else
                    return false; // invalid format: first element *must* be root tag

                break;

            case QXmlStreamReader::EndElement:
                //if current node has no parent, that means we just closed the root tag
                //we're done!
                if(!curNode->parent())
                {
                    delete curNode;
                    return true;
                }

                    //otherwise, we just closed the current category.
                    //on the next loop iteration, we should get either the start of the next category or the closing tag of the parent (either the parent category or the "parent" leaf name)
                else
                    curNode = (XmlNode*) QScopedPointer<XmlNode>(curNode)->parent();

                break;

            case QXmlStreamReader::Characters:
                if(!xml.isWhitespace())
                    map[curNode->fullPath()] = xml.text().toString();
                break;
        }
    }

    //if it gets here, an error occured.
    map.clear();
    return false;
}

bool writeSettingsXml(QIODevice &device, const QMap<QString, QVariant> &map)
{
    XmlNode *root = new XmlNode(rootName);


/************************************************************/
//step 1/2: process the structure of the settings map & the keys. the purpose
//of this step is to put all the keys of one category next to each other.
//but we do not sort within the category. in this step, we place our results
//from the QStringList of QMap.keys() into a tree-like structure
            foreach(const QString &unsplitKey, map.keys())
        {
            QStringList segs = unsplitKey.split("/", Qt::SkipEmptyParts);
            QString val = map[unsplitKey].toString();

            XmlNode *cur = root;

            //for each segment of the current key, we loop through the branches of
            //our tree looking for appropriate branches/leaves. on the way down to
            //the specific leaf we want, we create & add nodes as needed.

            for(int i = 0; i < segs.length(); i++)
            {
                if(i == segs.length() - 1)
                {
                    //the last segment is a leaf that wasn't previously found.
                    //we don't keep the ref since it becomes a child of the parent
                    new XmlNode(segs[i], val, cur);
                }
                else
                {
                    //search for the node for the current segment. create it as a
                    //child of the current node if it doesn't exist. then we use it
                    //for the next iteration
                    XmlNode *foundItem = 0;
                            foreach(QObject *object, cur->children())
                        {
                            XmlNode *child = (XmlNode*) object;
                            if(0 == QString::compare(child->tagName, segs[i], Qt::CaseInsensitive))
                            {
                                foundItem = child;
                                break;
                            }
                        }

                    if(!foundItem)
                        foundItem = new XmlNode(segs[i], QString(), cur);

                    cur = foundItem;
                }
            }
        }





/************************************************************/
//step 2/2: write processed data to xml file
//use a stack to implement writing the tree while converting it to a list.
//here's the general process:
// 1) the stack is actually used to traverse the levels of the tree.
//      loop through all entries in the stack while closing all tags.
//      when we find the sentinel '0': if the stack is emtpy, we're done!
//      otherwise, the children of one category are completed.
// 2) we write the start tag for the current item. we always push the sentinel
//      disregarding whether or not there are children.
// 3) if there are no children, we simply write the contents and continue.
//      because of the sentinel in the stack, the next iteration of the main
//      loop will close the tag. but, if there are children, we push them all
//      to the stack to output them before closing the parent tag. the subsequent
//      iterations of the main loop will handle all children and/or contents

    QXmlStreamWriter xml(&device);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(-1);
    xml.writeStartDocument();

    QList<XmlNode*> stack;
    stack << root;

    while(true)
    {
        //see step 1
        XmlNode *cur;
        while((cur = stack.takeLast()) == 0)
        {
            xml.writeEndElement();

            if(stack.isEmpty())
            {
                xml.writeEndDocument();
                delete root;
                return true;
            }
        }

        //see step 2
        xml.writeStartElement(cur->tagName);
        stack << 0; // required to close text-only elements as well as for nodes with children to go back up a level when children are processed.

        //see step 3
        if(cur->children().size() == 0)
            xml.writeCharacters(cur->subtext);
        else
            for(int i = 0; i < cur->children().length(); i++)
                stack << (XmlNode*) cur->children()[i];
    }

    //should *never* get here
    return false;
}