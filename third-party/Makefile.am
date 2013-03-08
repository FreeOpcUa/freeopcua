
gmock-1.6.0.zip:
	test -e gmock-1.6.0.zip || wget http://googlemock.googlecode.com/files/gmock-1.6.0.zip

gmock-1.6.0: gmock-1.6.0.zip
	test -e gmock-1.6.0 || (unzip gmock-1.6.0.zip && cd gmock-1.6.0 && ./configure --disable-shared && cd ..)

gmock: gmock-1.6.0
	test -e gmock || ln -s gmock-1.6.0 gmock

gtest-1.6.0.zip:
	test -e gtest-1.6.0.zip || wget http://googletest.googlecode.com/files/gtest-1.6.0.zip

gtest-1.6.0: gtest-1.6.0.zip
	test -e gtest-1.6.0 || (unzip gtest-1.6.0.zip && cd gtest-1.6.0 && ./configure --disable-shared && cd ..)

gtest : gtest-1.6.0
	test -e gtest || ln -s gtest-1.6.0 gtest

uripp-1.0.0.tar.gz:
	test -e uripp-1.0.0.tar.gz || wget http://citylan.dl.sourceforge.net/project/uripp/uripp/1.0.0/uripp-1.0.0.tar.gz

uripp-1.0.0: uripp-1.0.0.tar.gz
	test -e uripp-1.0.0 || (tar -xf uripp-1.0.0.tar.gz && cp patch/uri/* uripp-1.0.0/src && cd uripp-1.0.0 && autoreconf -f -i && ./configure --disable-shared && cd ..)

uri: uripp-1.0.0
	test -e uri || ln -s uripp-1.0.0 uri
	$(MAKE) -C uri

all: gmock gtest uri
	$(MAKE) -C gmock
	$(MAKE) -C gtest

check: gmock gtest
	$(MAKE) -C gmock
	$(MAKE) -C gtest
	$(MAKE) -C uri check

clean:
	if test -e gtest ; then make -C gtest clean ; fi
	if test -e gmock ; then make -C gmock clean ; fi
	if test -e uri   ; then make -C uri clean ; fi

distclean:
	rm -rf gmock* gtest* uri* Makefile

