DESCRIPTION = "com infrastructure"
SECTION = "automation"
LICENSE = "GPLv2+"

S = "${WORKDIR}/${PN}-${PV}"

SRC_URI = "http://ignum.dl.sourceforge.net/project/mozopc/${PN}/${PN}-${PV}.tar.gz"
#SRC_URI = "file:///home/treww/mozopc/branch/${PN}-${PV}/${PN}-${PV}.tar.gz"

EXTRA_OECONF = ""

DEPENDS = " boost gtest gmock sqlite3"

inherit autotools gettext

pkg_postinst_${PN} () {
#! /bin/sh -e
  if [ x"$D" = "x" ] ; then
    comregdir=${localstatedir}/lib/com
    comregdb=${comregdir}/registry.sqlite
    install -d ${comregdir}
    sqlite3 ${comregdb} "CREATE TABLE IF NOT EXISTS class_libraries (clsid  STRING PRIMARY KEY, library_path STRING);"
    COMREG_DATABASE_PATH=${comregdb} comreg --register --library=$(libdir)/libcom_test.so --clsid="ab40f7c8-c2f6-4c78-8093-8b0062a5ec28"
  else
    exit 1
  fi
}

