cmake_minimum_required(VERSION 3.0)

include(cmake/version.cmake)

project(
    slika
    DESCRIPTION "Image Comparison Tool"
    HOMEPAGE_URL "https://github.com/saierd/slika"
    LANGUAGES CXX)

# Dependencies.
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(X11 REQUIRED)
include(cmake/nautilus_extension.cmake)

include(cmake/clang_tidy.cmake)
include(cmake/compiler_flags.cmake)
include(cmake/package.cmake)

add_subdirectory(file_manager_integrations)

set(source_directory "src")
set(source_files
    external/icons.qrc
    src/command_line.h
    src/command_line.cpp
    src/file_helpers.cpp
    src/file_helpers.h
    src/image.cpp
    src/image.h
    src/image_difference.cpp
    src/image_difference.h
    src/main.cpp
    src/screenshot.cpp
    src/screenshot.h
    src/screenshot_file_manager.cpp
    src/screenshot_file_manager.h
    src/session.cpp
    src/session.h
    src/widgets/comparison_settings.cpp
    src/widgets/comparison_settings.h
    src/widgets/comparison_settings.ui
    src/widgets/comparison_view.cpp
    src/widgets/comparison_view.h
    src/widgets/empty_image.cpp
    src/widgets/empty_image.h
    src/widgets/empty_image.ui
    src/widgets/image_drop_widget.cpp
    src/widgets/image_drop_widget.h
    src/widgets/image_view.cpp
    src/widgets/image_view.h
    src/widgets/image_view.ui
    src/widgets/interactive_graphics_view.cpp
    src/widgets/interactive_graphics_view.h
    src/widgets/session_view.cpp
    src/widgets/session_view.h
    src/widgets/session_view.ui
    src/widgets/source_image_view.cpp
    src/widgets/source_image_view.h
    src/widgets/synchronizable_graphics_view.cpp
    src/widgets/synchronizable_graphics_view.h
    src/windows/main_window.cpp
    src/windows/main_window.h
    src/windows/main_window.ui)

add_executable(${PROJECT_NAME} ${source_files})
source_group(TREE "${source_directory}")
target_include_directories(${PROJECT_NAME} PRIVATE "${source_directory}")

target_link_libraries(${PROJECT_NAME} Qt5::Widgets X11::xcb)
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON)

install(TARGETS ${PROJECT_NAME})
