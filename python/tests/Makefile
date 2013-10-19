all:
	python setup.py bdist

check: all
	LD_LIBRARY_PATH=../../libuamappings/.libs PYTHONPATH=$(shell find . -name \*.so -exec dirname {} + ) python test.py
	
clean:
	rm -rvf build dist

dist:

distclean: clean

