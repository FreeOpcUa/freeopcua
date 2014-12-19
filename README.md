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
* Automatic generation of binary protocol from OPC schema files 

Not implemented yet (patches are welcome)
* Methods
* History
* Security: Authentication, Certificates handling, ..


##Usage

Documentation is sparse but several examples are availables:

C++ examples in https://github.com/FreeOpcUa/freeopcua/tree/master/src/examples  
Python examples in https://github.com/FreeOpcUa/freeopcua/tree/master/python/src  
https://github.com/FreeOpcUa/freeopcua/blob/master/python/tests/test_highlevel.py can also be a usefull source of information

### Example minimal client in python
```
  client = opcua.Client(False)
  client.connect("opc.tcp://10.10.10.4:4841/OPCUA/AnyServer/")

  objects = client.get_objects_node()
  print("Children of objects are: ", objects.get_children())

  var = objects.get_child(["3:AnObject", "AVariable"])
  print("Value of variable is: ", var.get_value())
  
  client.disconnect()
```


## Tested clients and servers with default settings
* uaexperts client application
* node-opcua (client and server tested)
* ignition server
* Beckhoff PLC (seems to be some issues, but working)
* ignition open-source sdk (server tested)
* quickopc client sdk
* prosysopc demo client (subscription against freeopcua server currently broken)
* prosysopc demo server 
* unified automation sdk (client and server tested)


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


