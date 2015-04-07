#!/usr/bin/env python

from distutils.core import setup, Extension
import sys
import os
import platform

opcua_server_path = os.environ.get('OPCUA_Server_PATH','..')

extra_compile_args = [
	'-std=c++11', 
  '-pthread',
  '-Wall',
  '--param=max-vartrack-size=0',
]

extra_link_args = [
  '-pthread',
]

include_dirs = [
  os.path.join(opcua_server_path,'include'),
]

library_dirs = [
  os.path.join(opcua_server_path,'.libs'),
  os.path.join(opcua_server_path,'lib'),
]

boost_library='boost_python3' if sys.version_info.major == 3 and platform.dist()[0] == 'fedora' else 'boost_python'

libraries = [
  boost_library,
  'opcuaclient',
  'opcuaserver',
]

setup(name='python-freeopcua',
  version='0.2',
  description='Python bindings to freeopcua.',
  author='Matthieu Bec',
  author_email='mdcb808@gmail.com',
  url='https://github.com/treww/opcua-python',
  license = 'LGPL',
  ext_modules = [
    Extension(
      name='opcua', 
      sources=[
        'src/py_opcua_module.cpp',
        'src/py_opcua_enums.cpp',
        'src/py_opcua_enums_ObjectId.cpp',
        'src/py_opcua_enums_StatusCode.cpp',
        'src/py_opcua_enums_AttributeId.cpp',
        'src/py_opcua_subscriptionclient.cpp',
      ],
      include_dirs = include_dirs,
      extra_compile_args = extra_compile_args,
      extra_link_args = extra_link_args,
      library_dirs = library_dirs,
      libraries = libraries,
    ),
  ]
)

