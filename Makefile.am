EXTRA_DIST = \
  src/module.cpp \
  tests/Makefile \
  tests/setup.py \
  tests/test.py \
  tests/test_computer.cpp \
  Makefile.am \
  Makefile.in \
  setup.py

opcua_paths = OPCUA_SERVER_PATH=$(OPCUA_SERVER_PATH)

all:
	$(opcua_paths) python setup.py build

check:
	$(opcua_paths) $(MAKE) -C tests check
	
clean:
	$(MAKE) -C tests clean
	rm -rvf build dist

python-dist:
	$(opcua_paths) python setup.py bdist

dist:

distclean: clean
	rm -vf Makefile

install: python-dist
	$(opcua_paths)python setup.py install --prefix=$(prefix)

