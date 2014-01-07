# Author: Alexander Rykovanov 2009
#
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

AC_INIT([opcuaserver], [0.1.2], [rykovanov.as@gmail.com])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([exe/src/server_main.cpp])
AM_INIT_AUTOMAKE([-Wall -Werror])

CXXFLAGS="-O0 -g -std=c++0x"

AC_PROG_CXX
AM_PROG_AR
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])


AC_ARG_WITH([gtest], 
            [AS_HELP_STRING([--with-gtest=DIR], [defines path to gtest root directory])],
            [ 
              AC_SUBST([GTEST_LIB], [-Wl,--whole-archive,$with_gtest/lib/.libs/libgtest.a,--no-whole-archive])
              AC_SUBST([GTEST_MAIN_LIB], [-Wl,--whole-archive,$with_gtest/lib/.libs/libgtest_main.a,--no-whole-archive])
              AC_SUBST([GTEST_INCLUDES], [-I$with_gtest/include])
              AC_MSG_NOTICE([Gtest path: $GTEST_INCLUDES]) 
            ], 
            [ 
              AC_SUBST([GTEST_LIB], [-lgtest])
              AC_SUBST([GTEST_MAIN_LIB], [-lgtest_main])
            ])

AC_ARG_WITH([gmock], 
            [AS_HELP_STRING([--with-gmock=DIR], [defines path to gmock root directory])],
            [ 
              AC_SUBST([GMOCK_LIBS], [-Wl,--whole-archive,$with_gmock/lib/.libs/libgmock.a,--no-whole-archive ])
              AC_SUBST([GMOCK_INCLUDES], [-I$with_gmock/include])
              AC_MSG_NOTICE([Gmock path: $GMOCK_INCLUDES]) 
            ], 
            [ 
              AC_SUBST([GMOCK_LIBS], [-lgmock])
            ])

AC_ARG_WITH([uamappings], 
            [AS_HELP_STRING([--with-uamappings=DIR], [defines path to uamappings root directory])],
            [ 
              AC_SUBST([UAMAPPINGS_LIBS], ["-L$with_uamappings/.libs -lopcuabinary"])
              AC_SUBST([UAMAPPINGS_INCLUDES], [-I$with_uamappings/include])
              AC_MSG_NOTICE([ua mappings path: $with_uamappings]) 
            ], 
            [ 
              AC_SUBST([UAMAPPING_LIBS], [-lopcuabinary])
            ])

AC_ARG_WITH([opccore],
            [AS_HELP_STRING([--with-opccore=DIR], [defines path to the root directory of libopccore library.])],
            [ 
              AC_SUBST([OPC_CORE_LIBS], ["-L$with_opccore/.libs -lopcuacore -lxml2"])
              AC_SUBST([OPC_CORE_INCLUDES], ["-I$with_opccore/include -I/usr/include/libxml2"])
              AC_MSG_NOTICE([opccore path: $with_opccore]) 
            ], 
            [ 
              AC_SUBST([OPC_CORE_INCLUDES], ["-I/usr/include/libxml2"])
              AC_SUBST([OPC_CORE_LIBS], [-lopcuacore])
            ])


AC_ARG_WITH([libopcuaclient], 
            [AS_HELP_STRING([--with-libopcuaclient=DIR], [defines path to the root directory of libopcua_client library.])],
            [ 
              AC_SUBST([OPC_CLIENT_LIBS], ["-L$with_libopcuaclient/.libs -lopcua_client"])
              AC_SUBST([OPC_CLIENT_INCLUDES], [-I$with_libopcuaclient/include])
              AC_MSG_NOTICE([libopcuaclient path: $with_libopcuaclient]) 
            ], 
            [ 
              AC_SUBST([OPC_CLIENT_LIBS], [-lopcua_client])
            ])


#check headers
AC_CHECK_HEADERS([unistd.h])

#################################################################
AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([address_space/Makefile])
AC_CONFIG_FILES([binary_protocol/Makefile])
AC_CONFIG_FILES([builtin_server/Makefile])
AC_CONFIG_FILES([endpoint_services/Makefile])
AC_CONFIG_FILES([exe/Makefile])
AC_CONFIG_FILES([services_registry/Makefile])
AC_CONFIG_FILES([standard_namespace/Makefile])
AC_CONFIG_FILES([tcp_server/Makefile])
AC_CONFIG_FILES([xml_address_space_loader/Makefile])
AC_OUTPUT

