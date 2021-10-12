# -*- coding: utf-8 -*-

import sys
import subprocess

try:
    from skbuild import setup
except ImportError:
    print(
        "Please update pip, you need pip 10 or greater,\n"
        " or you need to install the PEP 518 requirements in pyproject.toml yourself",
        file=sys.stderr,
    )
    raise

from setuptools import find_packages

# compilation of the DPU binary
# with open("./src/dpu_trees/mockup_dpu_binary", "w") as outfile:
#     subprocess.run(["echo", "something something"], stdout=outfile)
subprocess.run(
    ["dpu-upmem-dpurte-clang", "-o", "helloworld", "helloworld.c"],
    cwd="./src/dpu_trees/dpu_program",
)
subprocess.run(
    [
        "dpu-upmem-dpurte-clang",
        "-o",
        "trivial_checksum_example",
        "trivial_checksum_example.c",
    ],
    cwd="./src/dpu_trees/dpu_program",
)

# compilation of the host library
setup(
    name="dpu_trees",
    version="0.0.1",
    description="a package for tree algorithms on DPU",
    author="Sylvan Brocard",
    author_email="sylvan.brocard@gmail.com",
    url="https://github.com/SylvanBrocard/dpu_trees",
    download_url="https://github.com/SylvanBrocard/dpu_trees/archive/refs/tags/v0.0.1-alpha.tar.gz",
    license="MIT",
    packages=find_packages(where="src"),
    package_dir={"": "src"},
    cmake_install_dir="src/dpu_trees",
    include_package_data=True,
    extras_require={"test": ["pytest"]},
)
