set(CPACK_PACKAGE_VENDOR "Daniel Saier")

if(UNIX)
    set(CPACK_GENERATOR "DEB")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Daniel Saier <mail@danielsaier.de>")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
endif()

include(CPack)
