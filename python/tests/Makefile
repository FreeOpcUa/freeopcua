opcua_paths = OPCUA_SERVER_PATH=$(OPCUA_SERVER_PATH)
libraries_path =$(OPCUA_SERVER_PATH)/lib:$(OPCUA_SERVER_PATH)/.libs

all:
	$(opcua_paths) python setup.py bdist

TEST_VARS = $(opcua_paths) LD_LIBRARY_PATH=$(libraries_path) PYTHONPATH=$(shell find . -name \*.so -exec dirname {} + )

check: all
	$(TEST_VARS) python test.py
	
clean:
	rm -rvf build dist

dist:

distclean: clean

