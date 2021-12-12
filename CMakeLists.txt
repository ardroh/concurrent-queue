cmake_minimum_required (VERSION 3.16.3)
project (ConcurrentQueue)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0")

include_directories(src)

add_subdirectory(src)
# add_subdirectory(tests)