
cmake_minimum_required(VERSION 3.2)

project(wasabi-engine)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_ROOT src/wasabi)
set(INCLUDE_ROOT src/include)

find_package(Vulkan REQUIRED)

add_subdirectory(${SOURCE_ROOT})
if (UNIX)
    find_package(spdlog REQUIRED)
else()
    #set(SFML_DIR ${PROJECT_SOURCE_DIR}/external/SFML/cmake/)
    #set(SPDLOG_ROOT ${PROJECT_SOURCE_DIR}/external/spdlog/build/CMakeFiles/Export/lib/cmake/spdlog)
    #set(spdlog_DIR ${PROJECT_SOURCE_DIR}/external/spdlog/out/install/x64-Debug/lib/cmake/spdlog)
    #set(glm_DIR ${PROJECT_SOURCE_DIR}/external/glm/build/CMakeFiles/Export/lib/cmake/glm)

    add_subdirectory(external/spdlog)
    add_subdirectory(external/glfw)
    add_subdirectory(external/SFML)
    add_subdirectory(${SFML_ROOT})
endif()

