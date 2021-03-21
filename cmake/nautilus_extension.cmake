find_package(PkgConfig)
pkg_check_modules(NAUTILUS_EXTENSION REQUIRED libnautilus-extension)

add_library(nautilus-extension INTERFACE)
target_include_directories(nautilus-extension SYSTEM INTERFACE ${NAUTILUS_EXTENSION_INCLUDE_DIRS})
target_link_libraries(nautilus-extension INTERFACE ${NAUTILUS_EXTENSION_LIBRARIES})
