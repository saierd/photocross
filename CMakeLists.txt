cmake_minimum_required(VERSION 3.14)

include(cmake/version.cmake)

project(
    photocross
    DESCRIPTION "Image Comparison Tool"
    HOMEPAGE_URL "https://github.com/saierd/photocross"
    LANGUAGES CXX)

# Search for dependencies.
find_package(OpenMP REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(X11 REQUIRED)
include(cmake/nautilus_extension.cmake)

include(cmake/ccache.cmake)
include(cmake/clang_tidy.cmake)
include(cmake/compiler_flags.cmake)
include(cmake/package.cmake)

add_subdirectory(external)
add_subdirectory(file_manager_integrations)

add_executable(${PROJECT_NAME}
    resources/icons.qrc
    src/busy_dialog.cpp
    src/busy_dialog.h
    src/command_line.cpp
    src/command_line.h
    src/comparison_settings.cpp
    src/comparison_settings.h
    src/comparison_view_layers.cpp
    src/comparison_view_layers.h
    src/image.cpp
    src/image.h
    src/image_difference.cpp
    src/image_difference.h
    src/main.cpp
    src/utility/debounce.cpp
    src/utility/debounce.h
    src/utility/drag_drop.cpp
    src/utility/drag_drop.h
    src/utility/file.cpp
    src/utility/file.h
    src/utility/image.cpp
    src/utility/image.h
    src/utility/slider.cpp
    src/utility/slider.h
    src/utility/string_prefix.cpp
    src/utility/string_prefix.h
    src/utility/time.cpp
    src/utility/time.h
    src/screenshot.cpp
    src/screenshot.h
    src/screenshot_manager.cpp
    src/screenshot_manager.h
    src/session.cpp
    src/session.h
    src/settings.cpp
    src/settings.h
    src/widgets/color_picker.cpp
    src/widgets/color_picker.h
    src/widgets/comparison_settings_widget.cpp
    src/widgets/comparison_settings_widget.h
    src/widgets/comparison_settings_widget.ui
    src/widgets/comparison_view.cpp
    src/widgets/comparison_view.h
    src/widgets/empty_image.cpp
    src/widgets/empty_image.h
    src/widgets/empty_image.ui
    src/widgets/image_drop_widget.cpp
    src/widgets/image_drop_widget.h
    src/widgets/image_edit_menu.cpp
    src/widgets/image_edit_menu.h
    src/widgets/image_edit_menu.ui
    src/widgets/image_view.cpp
    src/widgets/image_view.h
    src/widgets/image_view.ui
    src/widgets/image_view_scene.cpp
    src/widgets/image_view_scene.h
    src/widgets/interactive_graphics_view.cpp
    src/widgets/interactive_graphics_view.h
    src/widgets/mouse_indicator.cpp
    src/widgets/mouse_indicator.h
    src/widgets/session_view.cpp
    src/widgets/session_view.h
    src/widgets/session_view.ui
    src/widgets/single_direction_scroll_area.cpp
    src/widgets/single_direction_scroll_area.h
    src/widgets/source_images.cpp
    src/widgets/source_images.h
    src/widgets/source_image_view.cpp
    src/widgets/source_image_view.h
    src/widgets/synchronizable_graphics_view.cpp
    src/widgets/synchronizable_graphics_view.h
    src/window.cpp
    src/window.h
    src/windows/main_window.cpp
    src/windows/main_window.h
    src/windows/main_window.ui)
source_group(TREE "src")
target_include_directories(${PROJECT_NAME} PRIVATE "src")

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenMP::OpenMP_CXX
    QHotkey::QHotkey
    Qt5::Widgets
    X11::xcb
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON)

install(TARGETS ${PROJECT_NAME})
