
cmake_minimum_required(VERSION 3.2)

# set the project name
project(wasabi-engine)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_BUILD_TYPE Debug)
set(SOURCE_ROOT src/wasabi)
set(INCLUDE_ROOT src/include)

set(spdlog_DIR ${PROJECT_SOURCE_DIR}/external/spdlog/out/install/x64-Debug/lib/cmake/spdlog)
#set(glm_DIR ${PROJECT_SOURCE_DIR}/external/glm/build/CMakeFiles/Export/lib/cmake/glm)
find_package(Vulkan REQUIRED)

#set(SPDLOG_ROOT ${PROJECT_SOURCE_DIR}/external/spdlog/build/CMakeFiles/Export/lib/cmake/spdlog)

#set(SFML_DIR ${PROJECT_SOURCE_DIR}/external/SFML/cmake/)

add_subdirectory(external/spdlog)
#add_subdirectory(external/glfw)
add_subdirectory(${SOURCE_ROOT})
add_subdirectory(external/SFML)
#add_subdirectory(${SFML_ROOT})

