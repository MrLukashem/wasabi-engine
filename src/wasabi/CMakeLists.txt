
set(COMPONENTS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/components/include)

add_subdirectory(rendering)
add_subdirectory(physics)
add_subdirectory(core)

include_directories(include)

set(BUILD_SHARED_LIBS OFF)
link_directories(${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/lib)
set(SFML_INCLUDE ${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/include)
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_USE_STATIC_STD_LIBS TRUE)
set(SFML_STATIC TRUE)
add_definitions(-DSFML_STATIC)

# add the executable
add_library(wasabi-engine-lib
    "Scene.cpp")

add_executable(wasabi-engine-main
    main.cpp)
#target_include_directories(wasabi-engine-lib PUBLIC include)
#target_include_directories(wasabi-engine-main PUBLIC include)

#include_directories(wasabi-engine-lib core/include)
include_directories(wasabi-engine-main core/include)
include_directories(wasabi-engine-main ${SFML_INCLUDE})

#target_link_libraries(wasabi-engine-lib STATIC {PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/lib/sfml-graphics.lib core)
target_link_libraries(wasabi-engine-lib wasabi-core-ecs sfml-window-s-d sfml-system-s-d opengl32 winmm gdi32 sfml-graphics-s-d core)
    
target_link_libraries(wasabi-engine-main wasabi-engine-lib)

