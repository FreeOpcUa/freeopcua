all:

check:
	$(MAKE) -C tests check
	
clean:
	$(MAKE) -C tests clean
	rm -rvf build dist

dist:
	python setup.py bdist

distclean: clean
	rm -rvf Makefile

