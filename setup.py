import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="cine-encoder",
    version="2.2",
    author="Oleg Kozhukharenko",
    author_email="depositmail@mail.ru",
    description="Encoder",
    keywords="video",
    license="GPLv3",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/CineEncoder/cine-encoder",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: Python Software Foundation License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
    requires=['pyqt5', 'pymediainfo'],
    data_files=[
        ("/usr/share/applications", [
            "cine_encoder/cine-encoder.desktop"
        ]),
        ("/usr/share/doc/cine-encoder", [
            "README.md"
        ]),
        ("/usr/share/icons/hicolor/64x64/apps", [
            "cine_encoder/cine-encoder.png"
        ]),
        ("/usr/share/licenses/cine-encoder", [
            "LICENSE"
        ]),
    ]
)
