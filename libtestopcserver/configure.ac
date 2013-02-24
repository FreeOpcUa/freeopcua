# Author: Alexander Rykovanov 2009
#
# Distributed under the GNU LGPL License
# (See accompanying file LICENSE or copy at 
# http://www.gnu.org/licenses/lgpl.html)
#

AC_INIT
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([src/addon.cpp])
AM_INIT_AUTOMAKE([libtestopcserver], [0.1.0], [rykovanov.as@gmail.com])

CXXFLAGS="-O0 -g -Wall -std=c++0x"

AC_PROG_CXX
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG([0.25])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADER([config.h])

#check headers
AC_CHECK_HEADERS([unistd.h])

##################################################################
# Linux Implementation of com.
AC_ARG_ENABLE([linux-com],
              [AS_HELP_STRING([--enable-linux-com], [enable building of linux implementation of com. yes or no (default is no) ])],
              [
                case "x$enable_linux_com" in
                xyes) AC_MSG_NOTICE([Supporting of linux com enabled.])
                ;;
                xno)
                ;;
                *)      AC_MSG_ERROR([Invalid argument of option --enable-linux-com. Should be yes or no.]) 
                ;;
                esac
              ],
              [
              ])

AM_CONDITIONAL([BUILD_LINUX_COM], [test x$enable_linux_com = xyes])

##################################################################
# xulrunner configuration

AC_ARG_ENABLE([xpcom],
              [AS_HELP_STRING([--enable-xpcom], [enable building of xpcom servers. yes or no (default is no) ])],
              [
                case "x$enable_xpcom" in
                xyes) AC_MSG_NOTICE([Supporting of xpcom enabled.])
                ;;
                xno)  AC_MSG_NOTICE([Supporting of xpcom disabled.])
                ;;
                *)    AC_MSG_ERROR([Invalid argument of option --enable-xpcom. Should be yes or no.]) 
                ;;
                esac
              ],
              [
              ])


AM_CONDITIONAL([BUILD_XPCOM], [test x$enable_xpcom = xyes])

AC_ARG_WITH([xulrunner-sdk], 
            [AS_HELP_STRING([--with-xulrunner-sdk=DIR], [defines path to xulrunner sdk. If not set used pkgconfig to find it.])],
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


AC_ARG_WITH([libcom], 
            [AS_HELP_STRING([--with-libcom=DIR], [defines path to the root directory of libcom library.])],
            [ 
              AC_SUBST([LIBCOM_LIBS], [-Wl,-L$with_libcom,--whole-archive,-lcomimpl,--no-whole-archive])
              AC_SUBST([LIBCOM_LIBRARY_PATH], [$with_libcom/.libs])
              AC_SUBST([LIBCOM_INCLUDES], [-I$with_libcom/include])
              AC_MSG_NOTICE([libcom path: $with_libcom]) 
            ], 
            [ 
              AC_SUBST([LIBCOM_LIBS], [-Wl,--whole-archive,-lcomimpl,--no-whole-archive])
            ])

AC_ARG_WITH([libopccore], 
            [AS_HELP_STRING([--with-libopccore=DIR], [defines path to the root directory of libopccore library.])],
            [ 
              AC_SUBST([OPC_CORE_LIBS], [-Wl,-L$with_libopccore,--whole-archive,-lopccore,--no-whole-archive])
              AC_SUBST([OPC_CORE_INCLUDES], [-I$with_libopccore/include])
              AC_MSG_NOTICE([libopccore path: $with_libopccore]) 
            ], 
            [ 
              AC_SUBST([OPC_CORE_LIBS], [-Wl,--whole-archive,-lopccore,--no-whole-archive])
            ])

AC_ARG_WITH([libopc], 
            [AS_HELP_STRING([--with-libopc=DIR], [defines path to root directory of libopc libary.])],
            [ 
              AC_SUBST([OPC_LIBS], [-Wl,-L$with_libopc,--whole-archive,-lopc,--no-whole-archive])
              AC_SUBST([OPC_INCLUDES], [-I$with_libopc/include])
              AC_MSG_NOTICE([libopc path: $with_libopc]) 
            ], 
            [ 
              AC_SUBST([OPC_LIBS], [-Wl,--whole-archive,-lopc,--no-whole-archive])
            ])

AC_ARG_WITH([opc-test], 
            [AS_HELP_STRING([--with-opc-test=DIR], [defines path to root directory of opc_test_linux sources.])],
            [ 
              AC_SUBST([OPC_TEST_PATH], [$with_opc_test/opctest])
              AC_MSG_NOTICE([opc_test_linux path: $OPC_TEST_PATH]) 
            ], 
            [ 
              AC_SUBST([OPC_TEST_PATH], [opctest])
            ])


#################################################################
AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([src/Makefile])

#linux com frontend libraries
AC_CONFIG_FILES([linux_com/Makefile])

#CPXOM frontend libraries
AC_CONFIG_FILES([xpcom/Makefile])

AC_OUTPUT

