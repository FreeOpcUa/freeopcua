# Author: Alexander Rykovanov 2009
#
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

AC_INIT([opcuaserver], [0.1.3], [rykovanov.as@gmail.com])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/serverapp/server_main.cpp])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

CXXFLAGS="-O0 -g -std=c++11"

AC_PROG_CXX
m4_ifdef([AM_PROG_AR],[AM_PROG_AR])
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])

# Setup custom BOOST paths
AC_ARG_WITH([boost],
   [AS_HELP_STRING(
        [--with-boost=PATH],
        [prefix where BOOST is installed @<:@default=/usr/local@:>@])],
   [BOOST_INSTALL_PATH=$withval],
   [BOOST_INSTALL_PATH=/usr/local])

LDFLAGS="$LDFLAGS -L$BOOST_INSTALL_PATH/lib"
CXXFLAGS="$CXXFLAGS -I$BOOST_INSTALL_PATH/include"

AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov],[Enable support of coverage analysis with gcov.])],
              [
                AC_SUBST([GCOV_FLAGS], ["-fprofile-arcs -ftest-coverage"])
                AC_SUBST([GCOV_LIBS], [-lgcov])
                AC_MSG_NOTICE([Enabled support of code coverage analysis.])
              ])

AC_ARG_ENABLE([python-bindings],
              [AS_HELP_STRING([--disable-python-bindings],[Disable building of python bindings.])],
              [
                enable_python_bindings=no
                AC_MSG_NOTICE([Disabled support of python bindings.])
              ],
              [
                enable_python_bindings=yes
              ])


AM_CONDITIONAL([ENABLE_PYTHON], [test "x$enable_python_bindings" == "xyes"])

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

AS_IF([test "x$enable_python_bindings" = "xyes"], [
	ac_configure_args="$ac_configure_args --with-opcuaserver"
	AC_CONFIG_SUBDIRS([python])
])

#################################################################
AC_CONFIG_FILES([debian/changelog])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([libopcuacore.pc])
AC_CONFIG_FILES([libopcuaprotocol.pc])
AC_CONFIG_FILES([libopcuaserver.pc])
AC_CONFIG_FILES([libopcuaclient.pc])
AC_OUTPUT

