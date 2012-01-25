# Python bindings for Linux GPIO access through sysfs
#
# Copyright (C) 2009  Volker Thoms <unconnected@gmx.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

PYTHON	?= python

all:
	$(PYTHON) setup.py build

install:
	$(PYTHON) setup.py install

test:
	$(PYTHON) test.py

clean:
	$(PYTHON) setup.py clean
	rm -rf build dist

cleandir distclean: clean
	$(PYTHON) setup.py clean -a



#PY_GPIO_DIR := py-gpio
#
#PYTHON ?= python
#DISTUTILS := \
#	cd $(PY_GPIO_DIR) && \
#	CPPFLAGS="$(CPPFLAGS) -I../include" $(PYTHON) setup.py

#all-python:
#	$(DISTUTILS) build

#clean-python:
#	$(DISTUTILS) clean
#	rm -rf py-gpio/build

#install-python:
#	$(DISTUTILS) install

#all: all-python

#clean: clean-python

#install: install-python
