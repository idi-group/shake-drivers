#!/usr/bin/env python

from distutils.core import setup, Extension
from distutils.command.install import INSTALL_SCHEMES
import sys, os
import datetime
today = datetime.date.today()

for scheme in INSTALL_SCHEMES.values():
    scheme['data'] = scheme['platlib']

setup(name='pyshake',
        version='%02d%02d%04d' % (today.day, today.month, today.year),
        description='Python module for SHAKE SK6/SK7',
        author='University of Glasgow',
        author_email='andrew.ramsay@glasgow.ac.uk',
        url='http://github.com/andrewramsay/shake-drivers',
        py_modules = ['pyshake', 'pyshake_sk7', 'pyshake_sk7_constants', 'pyshake_sk6', 'pyshake_sk6_constants', 'pyshake_constants', 'pyshake_serial_pc', 'pyshake_sk_common'],
        requires = ['serial'],
)
