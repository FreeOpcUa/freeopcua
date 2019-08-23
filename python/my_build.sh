#!/bin/bash
python2 setup.py clean
rm -rf build
export PATH=$PATH:/opt/buildroot-2017.02.5/output/host/usr/bin

export CC=arm-linux-gnueabihf-gcc
export LDSHARED="arm-linux-gnueabihf-gcc -shared"

sudo cp /usr/bin/g++ /usr/bin/g++.bak
sudo ln -sf /opt/buildroot-2017.02.5/output/host/usr/bin/arm-linux-gnueabihf-g++ /etc/alternatives/c++
#sudo ln -sf /opt/buildroot-2017.02.5/output/host/usr/bin/arm-linux-gnueabihf-g++ /usr/bin/g++

python2 setup.py build_ext --include-dirs="/home/jason/lib/boost:/home/jason/lib/arm-python/include/python2.7:/home/jason/lib/libxml2/build/include/libxml2:/home/jason/lib/libxml2/build/include" --library-dirs="/home/jason/lib/arm-python/lib:/home/jason/lib/libxml2/build/lib:/home/jason/lib/boost/stage/lib" --libraries="boost_atomic boost_python python2.7"

echo "======================build done==========================="
sudo ln -sf /usr/bin/g++ /etc/alternatives/c++
sudo cp /usr/bin/g++.bak /usr/bin/g++
OBJ=build/lib.linux-x86_64-2.7/opcua.so
arm-linux-gnueabihf-strip $OBJ
echo "======================cp to tftpboot======================="
cp $OBJ /tftpboot

