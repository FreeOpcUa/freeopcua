Open Source C++ OPC-UA Server and Client Library
========

LGPL OPC-UA server and client library written in C++. Python bindings can be found in the opcua-python repository.
The library is under development but works will all tested clients and servers.
http://freeopcua.github.io/

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
* Windows port (Help welcome to handle shared libraries...)

Not implemented yet (patches are welcome)
* Methods
* History
* Authentication
* Certificates handling


see examples in src/examples folder or the examples in python repository

### Windows

You need:
* VS2013
* cmake
* boost

### Ubuntu (Debians)
There is a script debian.soft in the root for installing all required soft to build all repositories.
Main point is the support of c++11: gcc-4.8+, clang 3.4

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

### Using GNU autotools

```sh
autoreconf -f -i
./configure --prefix=/path/to/server
make 
make check
make install
cd /path/to/server
LD_LIBRARY_PATH=./lib ./bin/opcuaserver
```
