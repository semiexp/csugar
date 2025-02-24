import os
import re
from setuptools import setup


SHARED_LIB = re.compile(r"pycsugar\..*\.(so|dylib|dll|pyd)")


build_dir = "build"
package_data = []
for file in os.listdir(build_dir):
    if SHARED_LIB.match(file):
        package_data.append(file)

assert len(package_data) == 1


setup(
    name="pycsugar",
    version="0.0.1",
    packages=[""],
    package_dir={"": build_dir},
    package_data={"": package_data},
)
