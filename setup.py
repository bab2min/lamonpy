import sys
from setuptools import setup, Extension
import numpy as np

setup(
    name='LatinLemmatizer',
    version="0.0.1",
    description='LatinLemmatizer',
    long_description='LatinLemmatizer',
    url='',
    author='',
    author_email='',
    ext_modules = [Extension('LatinLemmatizer',
        libraries=[],
        include_dirs=[np.get_include()],
        sources=['src/PyMain.cpp', 'src/LatinLemmatizer.cpp'],
        extra_compile_args=['--std=c++11', '-march=native', '-O3'],
        extra_link_args=['-pthread']
    )]
)
