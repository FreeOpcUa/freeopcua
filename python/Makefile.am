
EXTRA_DIST = \
  src/py_opcua_enums.cpp \
  src/py_opcua_enums.h \
  src/py_opcua_helpers.h \
  src/py_opcua_module.cpp \
  src/py_opcua_subscriptionclient.cpp \
  src/py_opcua_subscriptionclient.h \
  src/py_opcua_variant.h \
  tests/Makefile \
  tests/setup.py \
  tests/test.py \
  tests/test_computer.cpp \
  tests/test_highlevel.py \
  Makefile.am \
  Makefile.in \
  setup.py

opcua_paths = OPCUA_SERVER_PATH=$(OPCUA_SERVER_PATH)
libraries_path =$(OPCUA_SERVER_PATH)/lib:$(OPCUA_SERVER_PATH)/.libs
TEST_VARS = $(opcua_paths) LD_LIBRARY_PATH=$(libraries_path) PYTHONPATH=$(shell find ./build -name \*.so -exec dirname {} + )


all:
	echo OPCUASERVER_ROOT=$(OPCUASERVER_ROOT)
	$(opcua_paths) python $(srcdir)/setup.py build

#	$(opcua_paths) $(MAKE) -C tests check
check:
	$(TEST_VARS) python $(srcdir)/tests/test_highlevel.py

debug:
	$(TEST_VARS) cgdb --args python $(srcdir)/tests/test_highlevel.py
	

clean:
	$(MAKE) -C tests clean
	rm -rvf build dist

python-dist:
	$(opcua_paths) python $(srcdir)/setup.py bdist

dist:

distclean: clean
	rm -vf Makefile

install: python-dist
	$(opcua_paths) python $(srcdir)/setup.py install --prefix=$(prefix)

