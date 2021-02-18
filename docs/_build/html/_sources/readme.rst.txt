
============
Cine Encoder
============

About
-----

Cine Encoder for Linux is an application that allows you to convert media files while preserving HDR metadata. NVENC hardware encoding for NVIDIA video cards is also supported for H265 and H264 modes. The following encoding modes are implemented:

  - H265 NVENC (8, 10 bit)
  - H265 (8, 10 bit)
  - H264 NVENC (8 bit)
  - H264 (8 bit)
  - VP9 (10 bit)
  - AV1 (10 bit)
  - DNxHR HQX 4:2:2 (10 bit)
  - ProRes HQ 4:2:2 (10 bit)
  - ProRes 4444 4:4:4 (10 bit)

.. image:: ../images/View.png

.. image:: ../images/View_2.png


Installation
------------

Supported operating system: Linux x86_64

Make sure that the NVIDIA drivers are installed in accordance with the following requirements: `Using FFMPEG with NVIDIA. <https://docs.nvidia.com/video-technologies/video-codec-sdk/ffmpeg-with-nvidia-gpu/index.html>`_


- Arch Linux / Manjaro Linux:

  - install `AUR package <https://aur.archlinux.org/packages/cine-encoder/>`_ or cine-encoder-(ver)-arch_linux_x86_64.pkg.tar.xz file from `release <https://github.com/CineEncoder/cine-encoder/releases>`_


- Debian 10.6 / Ubuntu 20.04 / Linux Mint 20:

  - install file cine-encoder_(ver)_amd64.deb from `release <https://github.com/CineEncoder/cine-encoder/releases>`_


- Fedora 32:

  - to satisfy ffmpeg dependency Fedora users have to use `rpmfusion <https://rpmfusion.org/>`_ 
  - install file cine-encoder-(ver).fedora32.x86_64.rpm from `release <https://github.com/CineEncoder/cine-encoder/releases>`_


- CentOS 7.8:

  - install ffmpeg
  - install file cine-encoder-(ver).centos7.x86_64.rpm from `release <https://github.com/CineEncoder/cine-encoder/releases>`_


Buid instructions
-----------------

Install dependencies:

    - ffmpeg>=4.2
    - mkvtoolnix>=49.0
    - libmediainfo>=20.03
    - qt5-base>=5.15

Buld package:

.. code-block:: INI

    mkdir build
    
    cd build
    
    git clone https://github.com/CineEncoder/cine-encoder.git
    
    mv cine-encoder cine-encoder-(*ver)
    
    cd cine-encoder-(*ver)
    
    make

ver - current version


Licence
-------

GNU GPL v.3
See `LICENSE.md <https://github.com/CineEncoder/CineEncoder/blob/master/LICENSE>`_


Donate
------

PayPal
~~~~~~

If you wish to support this project, you can make a donation for $0.5 or more via `PayPal <https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU>`_. Your contributions will help keep the project alive and support future development.


Bitcoin
~~~~~~~

You can send bitcoins at this address or by scanning the QR code below:

.. image:: ../images/Bitcoin.png

1PqFt976YQTJ9Js3xym5GjtPDfsjq98kN8

