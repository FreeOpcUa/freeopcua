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
AC_CONFIG_SRCDIR([src/module.cpp])
AM_INIT_AUTOMAKE([libxpopc], [0.1], [rykovanov.as@gmail.com])

CXXFLAGS="-O0 -g -Wall -std=c++0x"

AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG([0.25])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])

#check headers
AC_CHECK_HEADERS([unistd.h])

AC_ARG_WITH([xulrunner-sdk], 
            [AS_HELP_STRING([--with-xulrunner-sdk=DIR], [defines path to xulrunner sdk. If not set it is used pkgconfig to find it.])],
            [ 
              case "x$with_xulrunner_sdk" in
              xyes)
                 AC_MSG_NOTICE([Tuning xulrunner via pkgconfig.])
                 #tune xulrunner with pkgconfig
                 PKG_CHECK_MODULES([XULRUNNER], [libxul > 2.0])
                 PKG_CHECK_MODULES([MOZILLA_JS], [mozilla-js > 2.0])
                 with_xulrunner_sdk=$(pkg-config --variable=sdkdir libxul) 
              ;;
              *)
                 AC_MSG_NOTICE([Using xulrunner at $with_xulrunner_sdk.])
                 #use specified xulrunner path
                 AC_SUBST([XULRUNNER_SDKDIR], [$with_xulrunner_sdk]) 
                 AC_SUBST([XULRUNNER_LIBDIR], [$with_xulrunner_sdk/lib])
                 AC_SUBST([XULRUNNER_INCLUDE_DIR], [$with_xulrunner_sdk/include])
                 AC_SUBST([MOZOPC_XULRUNNER_LIBS], [-Wl,-L$with_xulrunner_sdk/lib,--whole-archive,-lxpcomglue_s,-lxpcom,-lnspr4,-lmozalloc,-lxul,--no-whole-archive,-lc])
                 AC_SUBST([XULRUNNER_CFLAGS], ["-I$with_xulrunner_sdk/include -DXPCOM_EXPORTS -include xpcom-config.h"])
                 AC_SUBST([XULRUNNER_LDFLAGS], ["-L$with_xulrunner_sdk/lib -L$with_xulrunner_sdk/bin -Wl,-rpath-link,$with_xulrunner_sdk/bin -lxpcomglue_s -lxpcom -lnspr4"])
              ;;
              esac
            ], 
            [ 
            ])

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

AC_ARG_WITH([opc-core], 
            [AS_HELP_STRING([--with-opccore=DIR], [defines path to opccore sources root directory])],
            [ 
              AC_SUBST([OPC_CORE_LIBS], [-Wl,-L$with_opccore/.libs])
              AC_SUBST([OPC_CORE_INCLUDES], [-I$with_opccore])
              AC_MSG_NOTICE([libopc path: $with_opccore]) 
            ], 
            [ 
            ])

#################################################################
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

