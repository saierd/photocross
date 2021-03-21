project(slika-nautilus)

add_library(${PROJECT_NAME}
    SHARED
    entrypoint.cpp
    executor.cpp
    executor.h
    nautilus_extension.cpp
    nautilus_extension.h)
source_group(TREE ".")
target_link_libraries(${PROJECT_NAME} PRIVATE nautilus-extension)

# Disable warnings about C-style casts, because glib macros use them everywhere.
target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-old-style-cast")

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION "lib/slika/extensions/")
