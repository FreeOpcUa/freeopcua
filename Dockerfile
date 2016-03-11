FROM debian:jessie
MAINTAINER Beau Trepp <https://github.com/btrepp>
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get clean && apt-get update && apt-get install -y \
 gcc \
 g++ \
 make \
 autoconf \
 automake \
 libtool \
 libboost-dev \
 libboost-thread-dev \
 libboost-program-options-dev \
 libboost-system-dev \
 libboost-filesystem-dev \
 libcppunit-dev \
 pkg-config git \
 python-dev \
 libboost-python-dev \
 gsoap \
 libxml2-dev \
 build-essential \
 autotools-dev \
 dh-make \
 debhelper \
 devscripts \
 fakeroot \
 xutils \
 lintian \
 pbuilder \
 reprepro
ADD / /tmp/freeopcua
WORKDIR /tmp/freeopcua
RUN ./build.sh
RUN make check
RUN make install
