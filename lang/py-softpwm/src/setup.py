#!/usr/bin/env python

from distutils.core import setup, Extension

setup(	name="softpwm",
	version="0.0",
	description="Python bindings for Linux softpwm access through softpwm-dev",
	author="Volker Thoms",
	author_email="unconnected@gmx.de",
	maintainer="Volker Thoms",
	maintainer_email="unconnected@gmx.de",
	license="GPLv2",
	url="",
	include_dirs=["/usr/src/linux/include"],
	scripts=['scripts/softpwm_test.py'],
	ext_modules=[Extension("softpwm", ["softpwmmodule.c"])])
