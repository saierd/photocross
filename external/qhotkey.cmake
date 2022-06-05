set(QHOTKEY_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(qhotkey EXCLUDE_FROM_ALL)

# Disable warnings.
target_compile_options(qhotkey PRIVATE "-w")

# Disable printing of warning messages when registering a shortcut fails.
target_compile_definitions(qhotkey PRIVATE "QT_NO_WARNING_OUTPUT")

# Mark include directories as system include.
set_property(TARGET qhotkey PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(qhotkey INTERFACE SYSTEM qhotkey/QHotkey)
