DESCRIPTION = "com infrastructure"
SECTION = "automation"
LICENSE = "GPLv2+"

S = "${WORKDIR}/${PN}-${PV}"

SRC_URI = "http://ignum.dl.sourceforge.net/project/mozopc/${PN}/${PN}-${PV}.tar.gz"
#SRC_URI = "file:///home/treww/mozopc/branch/${PN}-${PV}/${PN}-${PV}.tar.gz"

EXTRA_OECONF = ""

DEPENDS = " boost gtest gmock libcom libopc"

inherit autotools gettext

