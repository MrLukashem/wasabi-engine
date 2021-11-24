
set(COMPONENTS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/components/include)
set(PLATFORM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/platform/include)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

#find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(utils)
add_subdirectory(platform)
add_subdirectory(ecs)
add_subdirectory(rendering)
add_subdirectory(physics)
add_subdirectory(core)

include_directories(include)

SET(CMAKE_CXX_FLAGS "-std=c++17")

#set(BUILD_SHARED_LIBS OFF)
#link_directories(${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/lib)
#set(SFML_INCLUDE ${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/include)

#set(SFML_STATIC TRUE)
#add_definitions(-DSFML_STATIC)

# add the executable
add_library(wasabi-engine-lib STATIC
    "Scene.cpp")

add_executable(wasabi-engine-main
    main.cpp)
#target_include_directories(wasabi-engine-lib PUBLIC include)
#target_include_directories(wasabi-engine-main PUBLIC include)

#include_directories(wasabi-engine-lib core/include)
#include_directories(wasabi-engine-main core/include)
#include_directories(wasabi-engine-main ${SFML_INCLUDE})

#target_link_libraries(wasabi-engine-lib STATIC {PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/lib/sfml-graphics.lib core)
#if (WIN32)
#    set(SFML_STATIC_LIBRARIES FALSE)
#    set(SFML_USE_STATIC_STD_LIBS TRUE)
#    target_link_libraries(wasabi-engine-lib
#            glm::glm
#            wasabi-ecs
#            wasabi-rendering
#            sfml-system
#            sfml-window
#            opengl32
#            winmm
#            gdi32
#            sfml-graphics
#            core)
#endif(WIN32)
#
if (UNIX)
    target_link_libraries(wasabi-engine-lib
            wasabi-ecs
            wasabi-rendering
            core)
endif(UNIX)
    
target_link_libraries(wasabi-engine-main wasabi-engine-lib)

