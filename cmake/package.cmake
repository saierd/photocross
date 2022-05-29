set(CPACK_PACKAGE_VENDOR "Daniel Saier")
set(CPACK_PACKAGE_VERSION "${project_version}")

if(UNIX)
    set(CPACK_GENERATOR "DEB")

    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Daniel Saier <mail@danielsaier.de>")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/package/debian/postinst"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/package/debian/prerm")
    set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION ON)

    install(
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/icon.svg"
        DESTINATION "share/pixmaps"
        RENAME "slika.svg")
    install(
        FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/package/slika.desktop"
        DESTINATION "share/applications")
endif()

include(CPack)
