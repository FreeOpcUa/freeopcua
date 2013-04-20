EXTRA_DIST = \
  patch/uri/utils.h \
  patch/uri/query.cpp \
  patch/uri/utils.cpp

#gmock-1.6.0.zip:
#	test -e gmock-1.6.0.zip || wget http://googlemock.googlecode.com/files/gmock-1.6.0.zip

#gmock-1.6.0: gmock-1.6.0.zip
#	test -e gmock-1.6.0 || (unzip gmock-1.6.0.zip && cd gmock-1.6.0 && ./configure --disable-shared && cd ..)

#gmock: gmock-1.6.0
#	test -e gmock || ln -s gmock-1.6.0 gmock

#gtest-1.6.0.zip:
#	test -e gtest-1.6.0.zip || wget http://googletest.googlecode.com/files/gtest-1.6.0.zip

#gtest-1.6.0: gtest-1.6.0.zip
#	test -e gtest-1.6.0 || (unzip gtest-1.6.0.zip && cd gtest-1.6.0 && ./configure --disable-shared && cd ..)

#gtest : gtest-1.6.0
#	test -e gtest || ln -s gtest-1.6.0 gtest

#all: gmock gtest uri

gmock/Makefile:
	cd gmock && ./configure && cd ..

gtest/Makefile:
	cd gtest && ./configure && cd ..

all:
#	$(MAKE) -C gmock
#	$(MAKE) -C gtest

check: gmock/Makefile gtest/Makefile
	$(MAKE) -C gmock
	$(MAKE) -C gtest

clean: gmock/Makefile gtest/Makefile
	$(MAKE) -C gtest clean
	$(MAKE) -C gmock clean


dist: gmock/Makefile gtest/Makefile
	$(MAKE) -C gtest distclean
	$(MAKE) -C gmock distclean

distclean: gmock/Makefile gtest/Makefile
	$(MAKE) -C gtest distclean
	$(MAKE) -C gmock distclean
	rm -rf Makefile

