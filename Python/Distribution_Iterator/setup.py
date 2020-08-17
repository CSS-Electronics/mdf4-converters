import platform
import setuptools

from pathlib import Path
from setuptools.dist import Distribution


# To force wheel to create a package with an ABI tag, alter the distribution such that is has knowledge of the
# external modules.
# https://stackoverflow.com/a/62667991
# https://stackoverflow.com/a/45150383
class DistributionWithTag(Distribution):
    def has_ext_modules(self):
        return True


# Extract information from the README file and embed it in the package.
readme_path = Path(__file__).absolute().parent / "README.md"
with open(readme_path, "r") as fh:
    long_description = fh.read()

# List of additional files to include (Not python files).
data_files = ["mdf_iter.pyi"]

# Include the correct extension depending on the platform the package is built on.
if platform.system() == "Windows":
    data_files.append("mdf_iter.pyd")
else:
    data_files.append("mdf_iter.so")

setuptools.setup(
    author="Magnus Aagaard Sørensen",
    author_email="mas@csselectronics.com",
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Development Status :: 3 - Alpha",
        "Environment :: Console",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3.5",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
    ],
    description="Iterator for MDF files containing bus logged data in CAN",
    distclass=DistributionWithTag,
    long_description=long_description,
    long_description_content_type="text/markdown",
    name="mdf_iter",
    package_data={
        "mdf_iter": data_files
    },
    packages=setuptools.find_packages(),
    python_requires='>=3.5',
    url="https://github.com/CSS-Electronics/mdf4-converters",
    version="0.0.1",
)
