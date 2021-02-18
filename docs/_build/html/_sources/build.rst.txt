
=================
Build from source
=================

Supported operating system: Linux x86_64

Make sure that the NVIDIA drivers are installed in accordance with the following requirements: `Using FFMPEG with NVIDIA. <https://docs.nvidia.com/video-technologies/video-codec-sdk/ffmpeg-with-nvidia-gpu/index.html>`_


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
