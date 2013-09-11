#!/usr/bin/python

from distutils.core import setup
from distutils.extension import Extension

sources = ['module.cpp'] 

includes = [
            '../../include',
            '../../../common/include',
            '../../../../libuamappings/include',
            '../../../../../libopccore/include']

cpp_flags = ['-std=c++11', 
             '-DMODULE_NAME=opcua', 
             '-Wl,-z,defs,-no-undefined']

libs = ['opccore',
        'opcuabinary',
        'opcua_client',
        'stdc++',
        'pthread',
        'boost_python']

ldirs = [
            '../../../../libuamappings',
            '../../../../../libopccore',
            '../..']

opcua_client = Extension(
    'opcua', 
    sources,
    include_dirs = includes,
    extra_compile_args = cpp_flags,
    library_dirs = ldirs,
    libraries = libs,
    language = 'c++')

modules = [opcua_client]

setup(name='opcua',
      version='0.1.2',
      description='Client interface for OPC UA servers.',
      author='Alexander Rykovanov',
      author_email='rykovanov.as@gmail.com',
      url='https://github.com/treww/opc_layer',
      license = 'LGPL',
      ext_modules = modules       
      )
