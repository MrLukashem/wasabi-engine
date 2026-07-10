
cmake_minimum_required(VERSION 3.5)

project(wasabi-engine)


set(CMAKE_CXX_STANDARD 20)
set(SOURCE_ROOT src/wasabi)
set(INCLUDE_ROOT src/include)
set(volk_DIR ${PROJECT_SOURCE_DIR}/external/volk/cmake)

find_package(Vulkan REQUIRED)

add_subdirectory(external/glm)
add_subdirectory(${SOURCE_ROOT})

if(APPLE)
    set(SPDLOG_ROOT ${PROJECT_SOURCE_DIR}/external/spdlog/build/CMakeFiles/Export/lib/cmake/spdlog)
    set(spdlog_DIR ${PROJECT_SOURCE_DIR}/external/spdlog/out/install/x64-Debug/lib/cmake/spdlog)
    set(glm_DIR ${PROJECT_SOURCE_DIR}/external/glm/build/CMakeFiles/Export/lib/cmake/glm)

    add_compile_definitions(VK_USE_PLATFORM_MACOS_MVK)
    add_compile_definitions(VK_USE_PLATFORM_METAL_EXT)
elseif(UNIX AND NOT APPLE)
    set(GLFW_USE_WAYLAND ON CACHE BOOL "Build GLFW with Wayland support" FORCE)
    #set(SFML_DIR ${PROJECT_SOURCE_DIR}/external/SFML/cmake/)
    #set(SPDLOG_ROOT ${PROJECT_SOURCE_DIR}/external/spdlog/build/CMakeFiles/Export/lib/cmake/spdlog)
    #set(spdlog_DIR ${PROJECT_SOURCE_DIR}/external/spdlog/out/install/x64-Debug/lib/cmake/spdlog)
    #set(glm_DIR ${PROJECT_SOURCE_DIR}/external/glm/build/CMakeFiles/Export/lib/cmake/glm)
else()
    set(SFML_DIR ${PROJECT_SOURCE_DIR}/external/SFML/cmake/)

    add_subdirectory(${SFML_ROOT})
endif()

add_subdirectory(external/spdlog)
add_subdirectory(external/glfw)
add_subdirectory(external/volk)
add_subdirectory(external/VulkanMemoryAllocator)
