# Author: Alexander Rykovanov 2009
#
# @license GNU LGPL
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#


AC_INIT
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/module.cpp])
AM_INIT_AUTOMAKE([libopc], [0.1.0], [rykovanov.as@gmail.com])

CXXFLAGS="-O0 -g -Wall -std=c++0x"

AC_PROG_CXX
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG([0.25])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])


AC_ARG_WITH([gtest], 
            [AS_HELP_STRING([--with-gtest=DIR], [defines path to gtest root directory])],
            [ 
              AC_SUBST([GTEST_LIBS], [-Wl,-L$with_gtest/lib/.libs])
              AC_SUBST([GTEST_INCLUDES], [-I$with_gtest/include])
              AC_MSG_NOTICE([Gtest path: $with_gtest]) 
            ], 
            [ 
            ])

AC_ARG_WITH([gmock], 
            [AS_HELP_STRING([--with-gmock=DIR], [defines path to gmock root directory])],
            [ 
              AC_SUBST([GMOCK_LIBS], [-Wl,-L$with_gmock/lib/.libs])
              AC_SUBST([GMOCK_INCLUDES], [-I$with_gmock/include])
              AC_MSG_NOTICE([Gmock path: $with_gmock]) 
            ], 
            [ 
            ])

AC_ARG_WITH([libcom], 
            [AS_HELP_STRING([--with-libcom=DIR], [defines path to libcom sources root directory])],
            [ 
              AC_SUBST([LIBCOM_LIBS], [-Wl,-L$with_libcom/.libs])
              AC_SUBST([LIBCOM_INCLUDES], [-I$with_libcom/include])
              AC_MSG_NOTICE([libcom path: $with_libcom]) 
            ], 
            [ 
            ])

AC_ARG_WITH([libopccore], 
            [AS_HELP_STRING([--with-libopccore=DIR], [defines path to libopccore sources root directory])],
            [ 
              AC_SUBST([OPC_CORE_LIBS], [-Wl,-L$with_libopccore/.libs])
              AC_SUBST([OPC_CORE_INCLUDES], [-I$with_libopccore/include])
              AC_MSG_NOTICE([opccore path: $with_libopccore]) 
            ], 
            [ 
            ])


#check headers
AC_CHECK_HEADERS([unistd.h])

#################################################################
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

