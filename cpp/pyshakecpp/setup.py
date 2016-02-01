#!/usr/bin/env python

import datetime
from distutils.core import setup, Extension
from distutils.command.install import INSTALL_SCHEMES
from platform import system
from sys import maxsize

today = datetime.date.today()

for scheme in INSTALL_SCHEMES.values():
	scheme['data'] = scheme['platlib']

os = system()
is_64bits = maxsize > 2 ** 32

if os == 'Windows':
    libs = ['shake_driver']
    if is_64bits:
        lib_dirs = ['../shake_driver/x64/Release']
    else:
        lib_dirs = ['../shake_driver/Release']
else:
    libs = ['shake_driver']
    lib_dirs = ['../shake_driver']

setup(name='shake',
		version='%02d%02d%04d' % (today.day, today.month, today.year),
		description='Python module for SHAKE SK6/SK7',
		author='University of Glasgow',
		author_email='andrew.ramsay@glasgow.ac.uk',
		url='http://github.com/andrewramsay/shake-drivers',
		py_modules = ['shake'],
		package_dir = {'':'./python_wrapper'},
		data_files = [('/usr/local/lib', ['../shake_driver/libshake_driver.so'])],
		ext_modules = 
		[
			Extension('pyshakecpp', ['src/pyshakecpp.cpp'], include_dirs=['../shake_driver/inc', 'inc'], library_dirs=lib_dirs, libraries=libs, define_macros = [('PYSHAKECPP_COMPILE', '1')]
			)
		]
)
