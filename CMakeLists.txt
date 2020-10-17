cmake_minimum_required(VERSION 3.0)

project(imgdiff CXX)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

include(cmake/compiler_flags.cmake)

set(SOURCE_DIRECTORY "src")
set(SOURCE_FILES
    src/image.h
    src/image_diff.cpp
    src/image_diff.h
    src/main.cpp
    src/utility/finally.h
    src/utility/signal_blocker.h
    src/widgets/interactive_graphics_view.cpp
    src/widgets/interactive_graphics_view.h
    src/windows/main_window.cpp
    src/windows/main_window.h
    src/windows/main_window.ui)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
source_group(TREE "src")
target_include_directories(${PROJECT_NAME} PRIVATE "${SOURCE_DIRECTORY}")

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON)
