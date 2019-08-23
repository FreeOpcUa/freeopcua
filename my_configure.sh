#!/bin/bash

export PATH=$PATH:/opt/buildroot-2017.02.5/output/host/usr/bin


./configure  CC=arm-linux-gnueabihf-gcc CXX=arm-linux-gnueabihf-g++ --host=arm-linux CPPFLAGS="-I/home/jason/lib/arm-python/include -I/home/jason/lib/libxml2/build/include/libxml2 -I/home/jason/lib/libxml2/build/include -I/home/jason/lib/boost" LDFLAGS="-L/home/jason/lib/arm-python/lib -L/home/jason/lib/libxml2/build/lib -L/home/jason/lib/boost/stage/lib" --enable-static=no --prefix=/home/jason/workspace/freeopcua/output

# After configure, DO EDIT Makefile
#   1. change LIBS
#   2. change /usr/include/libxmle to you libxml2 include path, use: %s/\/usr....
#   3. then do make


# Build python ext:
#   1. export CC=arm-linux-gnueabihf-gcc
#   2. export LDSHARED="arm-linux-gnueabihf-gcc -shared"
#   3. python2 setup.py build_ext --include-dirs="/home/jason/lib/boost:/home/jason/lib/arm-python/include/python2.7:/home/jason/lib/libxml2/build/include/libxml2:/home/jason/lib/libxml2/build/include" --library-dirs="/home/jason/lib/arm-python/lib:/home/jason/lib/libxml2/build/lib:/home/jason/lib/boost/stage/lib" --libraries=boost_atomic:boost_python:python2.7
