=================
Build from source
=================

|

Operating system: Linux x86_64

|

Install dependencies:

|

- Arch Linux / Manjaro Linux:
    - ffmpeg>=4.2
    - mkvtoolnix-cli>=49.0
    - qt5-base>=5.15
    - qt5-multimedia>=5.15
    - libmediainfo>=20.03

|

- Debian 10.6 / Ubuntu 20.04 / Linux Mint 20:
    - ffmpeg>=4.1.6
    - mkvtoolnix>=31.0
    - qt5-qmake>=5.11.3
    - qtbase5-dev>=5.11.3
    - qtmultimedia5-dev>=5.11.3
    - libmediainfo-dev>=18.12

|

- Fedora 33:
    - ffmpeg>=4.3.2
    - mkvtoolnix>=53.0.0
    - qt5-qtbase-devel>=5.15
    - qt5-qtmultimedia-devel>=5.15
    - libmediainfo-devel>=20.09

|

- CentOS 7.8:
    - ffmpeg>=3.4.8
    - mkvtoolnix>=46.0.0
    - qt5-qtbase-devel>=5.9.7
    - qt5-qtmultimedia-devel>=5.9.7
    - libmediainfo-devel>=21.03

|

Buld package:

.. code-block:: INI

 mkdir build
 cd build
 git clone https://github.com/CineEncoder/cine-encoder.git
 cd cine-encoder
 qmake-qt5 -o builddir/Makefile app/cine_encoder.pro -spec linux-g++ CONFIG+=qtquickcompiler (or for Debian:  /usr/lib/qt5/bin/qmake -o builddir/Makefile app/cine_encoder.pro -spec linux-g++)
 cd builddir
 make
