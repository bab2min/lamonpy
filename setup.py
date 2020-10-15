import sys
from setuptools import setup, Extension
import numpy as np
import os, os.path, struct, re, platform

sources = ['src/PyMain.cpp', 'src/Lemmatizer.cpp']
largs = ['-pthread']
arch_levels = {'':'', 'sse2':'-msse2', 'avx':'-mavx', 'avx2':'-mavx2 -mfma'}
if platform.system() == 'Windows': 
    cargs = ['/O2', '/MT', '/Gy']
    arch_levels = {'':'', 'sse2':'/arch:SSE2', 'avx':'/arch:AVX', 'avx2':'/arch:AVX2'}
elif platform.system() == 'Darwin': 
    cargs = ['-std=c++0x', '-O3', '-fpermissive', '-stdlib=libc++', '-Wno-unused-variable', '-Wno-switch']
    largs += ['-stdlib=libc++']
    if 'many' not in os.environ.get('AUDITWHEEL_PLAT', ''): arch_levels = {'':'-march=native'}
elif 'many' in os.environ.get('AUDITWHEEL_PLAT', ''):
    cargs = ['-std=c++0x', '-O3', '-fpermissive', '-g0', '-Wno-unused-variable', '-Wno-switch']
else:
    cargs = ['-std=c++0x', '-O3', '-fpermissive', '-Wno-unused-variable', '-Wno-switch']
    arch_levels = {'':'-march=native'}

if struct.calcsize('P') < 8: arch_levels = {k:v for k, v in arch_levels.items() if k in ('', 'sse2')}
else: arch_levels = {k:v for k, v in arch_levels.items() if k not in ('sse2',)}

modules = []
for arch, aopt in arch_levels.items():
    module_name = '_lamonpy' + ('_' + arch if arch else '')
    modules.append(Extension(module_name,
                    libraries=[],
                    include_dirs=['include', np.get_include()],
                    sources=sources,
                    define_macros=[('MODULE_NAME', 'PyInit_' + module_name)],
                    extra_compile_args=cargs + (aopt.split(' ') if aopt else []), extra_link_args=largs))

setup(
    name='lamonpy',
    version="0.2.0",
    description='Lamonpy, Latin POS Tagger & Lemmatizer for Python',
    long_description='',

    url='https://github.com/bab2min/lamonpy',

    author='bab2min',
    author_email='bab2min@gmail.com',

    license='MIT License',

    classifiers=[
        'Development Status :: 3 - Alpha',

        "Intended Audience :: Information Technology",
        "Intended Audience :: Science/Research",
        "Topic :: Software Development :: Libraries",
        "Topic :: Text Processing :: Linguistic",
        "Topic :: Scientific/Engineering :: Information Analysis",

        "License :: OSI Approved :: MIT License",

        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        "Operating System :: Microsoft :: Windows :: Windows Vista",
        "Operating System :: Microsoft :: Windows :: Windows 7",
        "Operating System :: Microsoft :: Windows :: Windows 8",
        "Operating System :: Microsoft :: Windows :: Windows 8.1",
        "Operating System :: Microsoft :: Windows :: Windows 10",
        "Operating System :: POSIX",
        "Operating System :: MacOS"
    ],
    install_requires=['py-cpuinfo', 'numpy>=1.10.0'],
    keywords='NLP,Latin',

    packages = ['lamonpy'],
    ext_modules=modules
)
