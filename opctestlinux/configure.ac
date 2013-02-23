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
AC_CONFIG_SRCDIR([src/main.cpp])
AM_INIT_AUTOMAKE([opctestlinux], [0.1.0], [rykovanov.as@gmail.com])
CXXFLAGS="-O0 -g -Wall -std=c++0x"
AC_PROG_CXX
PKG_PROG_PKG_CONFIG([0.25])
AC_CONFIG_MACRO_DIR([m4])
AC_DISABLE_STATIC
AC_PROG_LIBTOOL


AC_ARG_WITH([libcom], 
            [AS_HELP_STRING([--with-libcom=DIR], [defines path to the root directory of libcom library.])],
            [ 
              AC_SUBST([LIBCOM_LIBS], [-Wl,-L$with_libcom/.libs],--whole-archive,-lcom,--no-whole-archive)
              AC_SUBST([LIBCOM_INCLUDES], [-I$with_libcom/include])
              AC_MSG_NOTICE([libcom path: $with_libcom]) 
            ], 
            [ 
            ])

AC_ARG_WITH([libopc], 
            [AS_HELP_STRING([--with-libopc=DIR], [defines path to root directory of libopc libary.])],
            [ 
              AC_SUBST([OPC_LIBS], [-Wl,-L$with_libopc,--whole-archive,-lopc,--no-whole-archive])
              AC_SUBST([OPC_INCLUDES], [-I$with_libopc/include])
              AC_MSG_NOTICE([libopc path: $with_libopc]) 
            ], 
            [ 
            ])

AC_ARG_WITH([gtest], 
            [AS_HELP_STRING([--with-gtest=DIR], [defines path to gtest root directory])],
            [ 
              AC_SUBST([GTEST_LIBS], [-Wl,-L$with_gtest/lib/.libs,--whole-archive,-lgtest,--no-whole-archive])
              AC_SUBST([GTEST_INCLUDES], [-I$with_gtest/include])
              AC_MSG_NOTICE([Gtest path: $GTEST_INCLUDES]) 
            ], 
            [ 
              AC_SUBST([GTEST_LIBS], [-Wl,--whole-archive,-lgtest,--no-whole-archive])
            ])

AC_ARG_WITH([gmock], 
            [AS_HELP_STRING([--with-gmock=DIR], [defines path to gmock root directory])],
            [ 
              AC_SUBST([GMOCK_LIBS], [-Wl,-L$with_gmock/lib/.libs,--whole-archive,-lgmock,--no-whole-archive])
              AC_SUBST([GMOCK_INCLUDES], [-I$with_gmock/include])
              AC_MSG_NOTICE([Gmock path: $GMOCK_INCLUDES]) 
            ], 
            [ 
              AC_SUBST([GMOCK_LIBS], [-Wl,--whole-archive,-lgmock,--no-whole-archive])
            ])

#################################################################
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

