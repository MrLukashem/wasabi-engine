
add_library(wasabi-utils
	FileUtils.cpp "include/utils/defs.hpp")

SET(CMAKE_CXX_FLAGS "-std=c++17")

target_include_directories(wasabi-utils PUBLIC include)

