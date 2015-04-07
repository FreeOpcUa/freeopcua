#!/usr/bin/python

from distutils.core import setup
from distutils.extension import Extension
import os

opcua_server_path = os.environ['OPCUA_Server_PATH']

sources = [
  '../src/py_opcua_module.cpp',
  '../src/py_opcua_enums.cpp',
  '../src/py_opcua_subscriptionclient.cpp',
	'test_computer.cpp',
] 


includes = [
	opcua_server_path + '/include',
]

cpp_flags = [
	'-std=c++11', 
	'-DMODULE_NAME=test_opcua',
	'-Wl,--no-undefined',
]

libs = [
	'opcuaclient',
	'opcuaserver',
	'stdc++',
	'pthread',
	'boost_python',
]

ldirs = [
	opcua_server_path + '/lib',
	opcua_server_path + '/.libs',
]

opcua_client = Extension(
    'test_opcua', 
    sources,
    include_dirs = includes,
    extra_compile_args = cpp_flags,
    library_dirs = ldirs,
    libraries = libs,
    language = 'c++')

modules = [opcua_client]

setup(name='test_opcua',
      version='0.1.2',
      description='Client interface for OPC UA servers.',
      author='Alexander Rykovanov',
      author_email='rykovanov.as@gmail.com',
      url='https://github.com/treww/opcua-python',
      license = 'LGPL',
      ext_modules = modules       
      )

