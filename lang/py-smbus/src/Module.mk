# $Id: Makefile,v 1.6 2005/10/17 00:08:17 dugsong Exp $

PYTHON	= python

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


