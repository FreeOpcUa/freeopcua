#!/usr/bin/env python

from distutils.core import setup, Extension
import sys
import os
import platform

opcua_server_path = os.environ.get('OPCUA_SERVER_PATH','../freeopcua')

extra_compile_args = [
	'-std=c++11', 
  '-pthread',
  '-Wall',
]

extra_link_args = [
  '-pthread',
]

include_dirs = [
  os.path.join(opcua_server_path,'include'),
]

library_dirs = [
  os.path.join(opcua_server_path,'.libs'),
]

boost_library='boost_python3' if sys.version_info.major == 3 and platform.dist()[0] == 'fedora' else 'boost_python'

libraries = [
  boost_library,
  'opcuaclient',
  'opcuaserver',
]

setup(name='opcua',
  version='0.1.2',
  description='Python bindings to freeopcua.',
  author='Alexander Rykovanov',
  author_email='rykovanov.as@gmail.com',
  url='https://github.com/treww/opcua-python',
  license = 'LGPL',
  ext_modules = [
    Extension(
      name='opcua', 
      sources=[
        'src/py_opcua_module.cpp',
        'src/py_opcua_enums.cpp',
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

