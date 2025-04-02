#!/usr/bin/env python3
from distutils.core import setup
from setuptools import Extension, setup, find_packages
from Cython.Build import cythonize

# extensions = [Extension('wrapper',
#                         ['wrapper.pyx', 'attack.cpp'],
#                         language="c++",
#                         extra_link_args=["-lz"]
#                     )]
setup(
    ext_modules = cythonize([
        Extension("wrapper", ["wrapper.pyx"], extra_compile_args=[]  ),
    ]),
)

# setup(
#     name='wrapper', 
#     ext_modules=cythonize(extensions)
# )

