#!/usr/bin/env python

from setuptools import setup, find_packages
import sys, os
from codecs import open
from os import path

here = path.abspath(path.dirname(__file__))

# Get the long description from the README file
with open(path.join(here, '..', 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(
    name='shake-drivers',
    version='1.0.4',
    description='SHAKE sensor device driver',
    long_description=long_description,

    # The project's main homepage.
    url='https://github.com/andrewramsay/shake-drivers',

    # Author details
    author='Andrew Ramsay',
    author_email='andrew.ramsay@glasgow.ac.uk',

    # Choose your license
    license='BSD',

    # See https://pypi.python.org/pypi?%3Aaction=list_classifiers
    classifiers=[
        'Development Status :: 5 - Production/Stable',

        'Intended Audience :: Developers',

        'License :: OSI Approved :: BSD License',

        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
    ],

    keywords='development sensors sensing SHAKE',

    py_modules = ['pyshake', 'pyshake_sk7', 'pyshake_sk7_constants', 'pyshake_sk6', 'pyshake_sk6_constants', 'pyshake_constants', 'pyshake_serial_pc', 'pyshake_sk_common'],

    install_requires=['pyserial'],
)

