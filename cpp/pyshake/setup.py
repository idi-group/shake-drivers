#!/usr/bin/env python

from distutils.core import setup, Extension
from distutils.command.install import INSTALL_SCHEMES
import sys, os
import datetime
today = datetime.date.today()

for scheme in INSTALL_SCHEMES.values():
	scheme['data'] = scheme['platlib']

setup(name='shake',
		version='%02d%02d%04d' % (today.day, today.month, today.year),
		description='Python module for SHAKE SK6/SK7',
		author='University of Glasgow',
		author_email='adr@dcs.gla.ac.uk',
		url='http://code.google.com/p/shake-drivers',
		py_modules = ['shake'],
		package_dir = {'':'./python_wrapper'},
		data_files = [('/usr/local/lib', ['../shake_driver/libshake_driver.so'])],
		ext_modules = 
		[
			Extension('pyshake', ['src/pyshake.cpp'], include_dirs=['../shake_driver/inc', 'inc'], library_dirs=['../shake_driver'], libraries=['bluetooth', 'shake_driver'], define_macros = [('PYSHAKE_COMPILE', '1')]
			)
		]
)
