#!/bin/sh
tar -xvf opcuaserver-${VERSION}.tar.gz
cd opcuaserver-${VERSION}
dh_make -y -l -c lgpl3 -e rykovanov.as@gmail.com -f ../opcuaserver-${VERSION}.tar.gz
dpkg-buildpackage -rfakeroot -nc -F ${DPKG_BUILDPACKAGE_OPTS}

