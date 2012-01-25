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
