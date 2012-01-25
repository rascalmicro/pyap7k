#!/usr/bin/env python

from distutils.core import setup, Extension

setup(	name="gpio",
	version="0.0",
	description="Python bindings for Linux GPIO access through sysfs",
	author="Volker Thoms",
	author_email="unconnected@gmx.de",
	maintainer="Volker Thoms",
	maintainer_email="unconnected@gmx.de",
	license="GPLv2",
	url="http://www.hs-augsburg.de/~vthoms",
	scripts=['scripts/callback.py','scripts/callback_obj.py', 'scripts/py-gpiotest.py'],
	ext_modules=[Extension("gpio", ["gpiomodule.c"])])
