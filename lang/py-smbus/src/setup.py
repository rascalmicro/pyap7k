#!/usr/bin/env python

from distutils.core import setup, Extension
from distutils.debug import DEBUG
import sys, os

if os.environ.get('CROSS_COMPILING') == 'yes':
	print "cross compiling detected"
else:
	print "NOT cross compiling detected"

setup(	name="smbus",
	version="1.1",
	description="Python bindings for Linux SMBus access through i2c-dev",
	author="Mark M. Hoffman",
	author_email="mhoffman@lightlink.com",
	maintainer="Mark M. Hoffman",
	maintainer_email="linux-i2c@vger.kernel.org",
	license="GPLv2",
	url="http://lm-sensors.org/",
	scripts=['scripts/tinyread.py','scripts/pca9555_test.py','scripts/DS1803_test.py','scripts/DS1803.py','scripts/DS1803_dimmLED.py','scripts/DS1803_direct.py'],
	ext_modules=[Extension("smbus", ["smbusmodule.c"])])
