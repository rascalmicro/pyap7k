#!/usr/bin/env python

from distutils.core import setup, Extension

setup(	name="spi",
	version="1.1",
	description="Python bindings for Linux SPI access through spi-dev",
	author="Volker Thoms",
	author_email="unconnected@gmx.de",
	maintainer="Volker Thoms",
	maintainer_email="unconnected@gmx.de",
	license="GPLv2",
	url="http://www.hs-augsburg.de/~vthoms",
	include_dirs=["/usr/src/linux/include"],
	scripts=['scripts/93LC46_LAtest.py','scripts/93LC46_test2.py','scripts/93LC46_test.py','scripts/ds1305_test.py','scripts/mcp3304.py','scripts/mcp3304_test.py','scripts/mcp4922.py','scripts/spitest3.py'],
	ext_modules=[Extension("spi", ["spimodule.c"])])
