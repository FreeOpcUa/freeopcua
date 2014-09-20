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
libraries_path =$(OPCUA_SERVER_PATH)/lib:$(OPCUA_SERVER_PATH)/.libs
TEST_VARS = $(opcua_paths) LD_LIBRARY_PATH=$(libraries_path) PYTHONPATH=$(shell find ./build -name \*.so -exec dirname {} + )


all:
	$(opcua_paths) python setup.py build

#	$(opcua_paths) $(MAKE) -C tests check
check:
	$(TEST_VARS) python ./tests/test_highlevel.py

debug:
	$(TEST_VARS) cgdb --args python ./tests/test_highlevel.py
	

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

