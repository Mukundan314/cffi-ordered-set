from setuptools import setup

setup(
    name="cffi-ordered-set",
    version="0.0.1",
    cffi_modules=["builder.py:ffibuilder"],
    install_requires=["cffi>=1.0.0"],
)
