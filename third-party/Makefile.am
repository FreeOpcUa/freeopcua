gmock/Makefile:
	cd gmock && ./configure && cd ..

gtest/Makefile:
	cd gtest && ./configure && cd ..

check: gmock/Makefile gtest/Makefile
	$(MAKE) -C gmock
	$(MAKE) -C gtest

clean:
	test -e gtest/Makefile && $(MAKE) -C gtest clean || true
	test -e gmock/Makefile && $(MAKE) -C gmock clean || true


dist: gmock/Makefile gtest/Makefile
	$(MAKE) -C gtest dist
	$(MAKE) -C gmock dist

distclean:
	$(MAKE) -C gtest distclean || true
	$(MAKE) -C gmock distclean || true
	rm -rf Makefile

