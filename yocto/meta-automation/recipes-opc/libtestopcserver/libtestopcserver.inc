DESCRIPTION = "com infrastructure"
SECTION = "automation"
LICENSE = "GPLv2+"

S = "${WORKDIR}/${PN}-${PV}"

SRC_URI = "http://ignum.dl.sourceforge.net/project/mozopc/${PN}/${PN}-${PV}.tar.gz"
#SRC_URI = "file:///home/treww/mozopc/branch/${PN}-${PV}/${PN}-${PV}.tar.gz"

EXTRA_OECONF = " --enable-linux-com"

DEPENDS = " boost libcom libopccore libopc"

inherit autotools gettext

pkg_postinst_${PN} () {
#! /bin/sh -e
  if [ x"$D" = "x" ] ; then
    comreg --register --library=$(libdir)/${PN}.so --clsid="ed73f3ef-abeb-4677-9b3b-49e83c122ff3"
  else
    exit 1
  fi
}

