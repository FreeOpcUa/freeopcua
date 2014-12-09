Open Source C++ OPC-UA Server and Client Library
========
[![Build Status](https://travis-ci.org/FreeOpcUa/freeopcua.svg?branch=master)](https://travis-ci.org/FreeOpcUa/freeopcua)

LGPL OPC-UA server and client library written in C++.  
Python bindings can be found in the python directory.

code: https://github.com/FreeOpcUa/freeopcua  
website: http://freeopcua.github.io/  
mailing list: https://groups.google.com/forum/#!forum/freeopcua  


Currently supported:
* read, write, browse
* translateBrowsePathToNodeId
* DataChange Events
* Events
* Subscriptions

Work in progress
* StatusChange Events
* Filtering
* Documentation
* Windows port 

Not implemented yet (patches are welcome)
* Methods
* History
* Security: Authentication, Certificates handling, ..


##Installation

An environment supporting c++11 is necessary: gcc-4.8+, clang 3.4 or VS2013


#### Ubuntu (Debians)
There is a script debian.soft in the root for installing all required soft to build all repositories.

#### Using GNU autotools

```sh
autoreconf -f -i
./configure --prefix=/path/to/server
make 
make check
make install
cd /path/to/server
LD_LIBRARY_PATH=./lib ./bin/opcuaserver
```
### Using cmake

#### Linux
```sh
mkdir build
cd build
cmake ..
make
make test
```
#### Windows
Boost and libxml2 are necessary
```
rem compiling
mkdir build
cd build
cmake .. -DBOOST_ROOT=c:\boost_1_56
cmake --build
ctest -C Debug
cd bin\Debug
example_server.exe
```

##Usage

see C++ examples in src/examples folder or Python examples in python/examples. test_highlevel.py in python/tests can also be a usefull source of information


## Tested clients and servers with default settings
* uaexperts client application
* node-opcua (client and server tested)
* ignition server
* ignition open-source sdk (server tested)
* quickopc client sdk
* prosysopc demo client (subscription against freeopcua server currently broken)
* prosysopc demo server 
* unified automation sdk (client and server tested)
