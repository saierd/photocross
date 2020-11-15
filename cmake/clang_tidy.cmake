find_program(CLANG_TIDY_EXE NAMES
    "clang-tidy-11"
    "clang-tidy-10"
    "clang-tidy-9"
    "clang-tidy-8"
    "clang-tidy")

if (CLANG_TIDY_EXE)
    option(CLANG_TIDY "Enable clang-tidy" ON)
    if (CLANG_TIDY)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()
else()
    message(WARNING "Could not find clang-tidy.")
endif()
