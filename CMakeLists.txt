
cmake_minimum_required(VERSION 3.10)

# set the project name
project(wasabi-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(SOURCE_ROOT src/wasabi)
set(INCLUDE_ROOT src/include)

add_subdirectory(${SOURCE_ROOT})

