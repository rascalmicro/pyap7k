#!/usr/bin/env python

from distutils.core import setup, Extension

setup(	name="pwm",
	version="0.0",
	description="Python bindings to control PWM unit on AP700x through sysfs",
	author="Volker Thoms",
	author_email="unconnected@gmx.de",
	maintainer="Volker Thoms",
	maintainer_email="unconnected@gmx.de",
	license="GPLv2",
	url="http://www.hs-augsburg.de/~vthoms",
	scripts=['scripts/pwmtest.py'],
	ext_modules=[Extension("pwm", ["pwmmodule.c"])])
