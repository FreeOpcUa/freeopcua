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
opcua_paths = OPCUA_SERVER_PATH=$(OPCUA_SERVER_PATH)
libraries_path =$(OPCUA_SERVER_PATH)/lib
TEST_VARS = $(opcua_paths) LD_LIBRARY_PATH=$(libraries_path) PYTHONPATH=$(shell find ./build -name \*.so -exec dirname {} + )


all:
	$(opcua_paths) python setup.py build

check:
	$(opcua_paths) $(MAKE) -C tests check
	$(TEST_VARS) python ./tests/test_highlevel.py
	
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

