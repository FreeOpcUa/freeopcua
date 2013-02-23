DESCRIPTION = "Google mock c++ framework"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://COPYING;md5=cbbd27594afd089daa160d3a16dd515a"


SRC_URI = "http://googlemock.googlecode.com/files/gmock-1.6.0.zip"
SRC_URI[md5sum] = "f547f47321ca88d3965ca2efdcc2a3c1"
SRC_URI[sha256sum] = "341ad07c31c619ba32c88c2265bc23693da1df062f47f146a7ed340f646d82b7"

inherit autotools binconfig



do_install() {
        install -d ${D}${includedir}/gmock
        install ${S}/include/gmock/*.* ${D}${includedir}/gmock
        
        install -d ${D}${includedir}/gmock/internal
        install ${S}/include/gmock/internal/*.* ${D}${includedir}/gmock/internal
        
        install -d ${D}${libdir}
        install ${S}/lib/.libs/libgmock.a ${D}${libdir}/libgmock.a
        install ${S}/lib/.libs/libgmock_main.a ${D}${libdir}/libgmock_main.a
}

