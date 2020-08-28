TARGET = all

.PHONY: all deb rpm clean

$(TARGET): deb rpm

appimage:
	echo 'not implemented yet'

deb:
	python3 setup.py --command-packages=stdeb.command sdist_dsc --with-python3=True bdist_deb

rpm:
	python3 setup.py bdist_rpm

clean:
	rm -fr deb_dist/ dist/ build/ *.egg-info/ cine-encoder-*.tar.gz
