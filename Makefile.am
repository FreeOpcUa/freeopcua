EXTRA_DIST = \
  src/module.cpp \
  tests/Makefile \
  tests/setup.py \
  tests/test.py \
  tests/test_computer.cpp \
  Makefile.am \
  Makefile.in \
  setup.py


all:
	python setup.py build

check:
	$(MAKE) -C tests check
	
clean:
	$(MAKE) -C tests clean
	rm -rvf build dist

python-dist:
	python setup.py bdist

dist:

distclean: clean
	rm -rvf Makefile

install: python-dist
	python setup.py install
