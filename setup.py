#!/usr/bin/env python

from distutils.core import setup, Extension
import os

opcua_server_path = os.environ.get('OPCUA_SERVER_PATH','../freeopcua')

extra_compile_args = [
	'-std=c++11', 
  '-pthread',
  '-Wall',
]

extra_link_args = [
  '-pthread'
]

include_dirs = [
  os.path.join(opcua_server_path,'include')
]

library_dirs = [
  os.path.join(opcua_server_path,'.libs'),
]

libraries = [
  'boost_python',
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
        'src/wrap_opcua_module.cpp',
        'src/wrap_opcua_enums.cpp',
        'src/wrap_opcua_subscriptionclient.cpp',
      ],
      include_dirs = include_dirs,
      extra_compile_args = extra_compile_args,
      extra_link_args = extra_link_args,
      library_dirs = library_dirs,
      libraries = libraries,
    ),
  ]
)

