set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wnull-dereference -Wold-style-cast -Wshadow")
# Warnings recommended by Abseil (see https://abseil.io/docs/cpp/platforms/compilerflags).
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual -Wconversion-null -Woverlength-strings -Wpointer-arith -Wunused-local-typedefs -Wunused-result -Wvarargs -Wvla -Wwrite-strings")
