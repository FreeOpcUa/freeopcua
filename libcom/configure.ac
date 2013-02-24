# Author: Alexander Rykovanov 2009
#
#
# @license GNU LGPL
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

AC_INIT
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/com.cpp])
AM_INIT_AUTOMAKE([libcom], [0.9.3], [rykovanov.as@gmail.com])

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
              AC_MSG_NOTICE([Gtest path: $GTEST_INCLUDES]) 
            ], 
            [ 
            ])

AC_ARG_WITH([gmock], 
            [AS_HELP_STRING([--with-gmock=DIR], [defines path to gmock root directory])],
            [ 
              AC_SUBST([GMOCK_LIBS], [-Wl,-L$with_gmock/lib/.libs])
              AC_SUBST([GMOCK_INCLUDES], [-I$with_gmock/include])
              AC_MSG_NOTICE([Gmock path: $GMOCK_INCLUDES]) 
            ], 
            [ 
            ])


#check headers
AC_CHECK_HEADERS([unistd.h])

#################################################################
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

