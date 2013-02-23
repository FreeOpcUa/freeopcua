# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU GPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/copyleft/gpl.html)
#
# $Id:  $
# $Date: $
# $Revision: $

AC_INIT
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/opc_client/server.cpp])
AM_INIT_AUTOMAKE([libopccore], [0.1.0], [rykovanov.as@gmail.com])

CXXFLAGS="-O0 -g -Wall -std=c++0x"

AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])

#check headers
AC_CHECK_HEADERS([unistd.h])


#################################################################
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

