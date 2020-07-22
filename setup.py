import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="cine-encoder-helg1980", # Replace with your own username
    version="1.0",
    author="Oleg Kozhukharenko",
    author_email="depositmail@mail.ru",
    description="Encoder",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/CineEncoder/CineEncoder",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: GPL License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.6',
)
