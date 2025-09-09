
add_library(wasabi-utils STATIC
	FileUtils.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++20")

target_include_directories(wasabi-utils PUBLIC include)

