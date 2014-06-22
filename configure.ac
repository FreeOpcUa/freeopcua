# Author: Alexander Rykovanov 2009
#
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

AC_INIT([opcuaserver], [0.1.3], [rykovanov.as@gmail.com])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/server/server_main.cpp])
AM_INIT_AUTOMAKE([-Wall -Werror])

CXXFLAGS="-O0 -g -std=c++11"

AC_PROG_CXX
AM_PROG_AR
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])

AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov],[Enable support of coverage analize with gcov.])],
              [
                AC_SUBST([GCOV_FLAGS], ["-fprofile-arcs -ftest-coverage"])
                AC_SUBST([GCOV_LIBS], [-lgcov])
                AC_MSG_NOTICE([Enabled support of code coverage analize.])
              ],
              [
              ])

AC_SUBST([GTEST_LIB], [-Wl,--whole-archive,$srcdir/tests/gtest/lib/.libs/libgtest.a,--no-whole-archive])
AC_SUBST([GTEST_MAIN_LIB], [-Wl,--whole-archive,$srcdir/tests/gtest/lib/.libs/libgtest_main.a,--no-whole-archive])
AC_SUBST([GTEST_INCLUDES], [-I$srcdir/tests/gtest/include])
AC_SUBST([GMOCK_LIBS], [-Wl,--whole-archive,$srcdir/tests/gmock/lib/.libs/libgmock.a,--no-whole-archive ])
AC_SUBST([GMOCK_INCLUDES], [-I$srcdir/tests/gmock/include])

AC_SUBST([RELEASE_DATE], [$(date -R)])

#check headers
AC_CHECK_HEADERS([unistd.h])

#################################################################
AC_CONFIG_SUBDIRS([tests/gtest])
AC_CONFIG_SUBDIRS([tests/gmock])

#################################################################
AC_CONFIG_FILES([debian/changelog])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libopcuacore.pc])
AC_CONFIG_FILES([libopcuaprotocol.pc])
AC_CONFIG_FILES([libopcuaserver.pc])
AC_CONFIG_FILES([libopcuaclient.pc])
AC_OUTPUT

