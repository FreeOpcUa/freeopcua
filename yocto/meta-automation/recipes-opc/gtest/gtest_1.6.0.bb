DESCRIPTION = "Google test c++ testing framework"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://COPYING;md5=cbbd27594afd089daa160d3a16dd515a"

SRC_URI = "http://googletest.googlecode.com/files/gtest-1.6.0.zip"
SRC_URI[md5sum] = "4577b49f2973c90bf9ba69aa8166b786"
SRC_URI[sha256sum] = "5ec97df8e75b4ee796604e74716d1b50582beba22c5502edd055a7e67a3965d8"

inherit autotools binconfig


GTEST_HEADERS="${S}/include/gtest/*.* ${S}/include/gtest/internal/*.*"
GTEST_LIBRARIES="${S}/lib/.libs/libgtest.a"


do_install() {
        install -d ${D}${includedir}/gtest
        install ${S}/include/gtest/*.* ${D}${includedir}/gtest
        
        install -d ${D}${includedir}/gtest/internal
        install ${S}/include/gtest/internal/*.* ${D}${includedir}/gtest/internal
        
        install -d ${D}${libdir}
        install ${GTEST_LIBRARIES} ${D}${libdir}/libgtest.a
}

