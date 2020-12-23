cmake_minimum_required(VERSION 3.0)

project(imgdiff CXX)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include(cmake/clang_tidy.cmake)
include(cmake/compiler_flags.cmake)

set(SOURCE_DIRECTORY "src")
set(SOURCE_FILES
    external/icons.qrc
    src/image.cpp
    src/image.h
    src/image_difference.cpp
    src/image_difference.h
    src/main.cpp
    src/session.cpp
    src/session.h
    src/widgets/comparison_settings.cpp
    src/widgets/comparison_settings.h
    src/widgets/comparison_settings.ui
    src/widgets/image_view.cpp
    src/widgets/image_view.h
    src/widgets/image_view.ui
    src/widgets/interactive_graphics_view.cpp
    src/widgets/interactive_graphics_view.h
    src/widgets/session_view.cpp
    src/widgets/session_view.h
    src/widgets/session_view.ui
    src/widgets/synchronizable_graphics_view.cpp
    src/widgets/synchronizable_graphics_view.h
    src/windows/main_window.cpp
    src/windows/main_window.h
    src/windows/main_window.ui)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
source_group(TREE "${SOURCE_DIRECTORY}")
target_include_directories(${PROJECT_NAME} PRIVATE "${SOURCE_DIRECTORY}")

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON)
