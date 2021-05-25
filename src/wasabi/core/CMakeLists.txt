
add_subdirectory(ecs)

find_package(spdlog REQUIRED)
find_package(glm REQUIRED)

include_directories(include/core)
set(SFML_INCLUDE ${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/include)
include_directories(wasabi-engine-main ${SFML_INCLUDE})
set(BUILD_SHARED_LIBS OFF)
link_directories(${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/lib)
set(SFML_INCLUDE ${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/include)
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_USE_STATIC_STD_LIBS TRUE)
set(SFML_STATIC TRUE)
add_definitions(-DSFML_STATIC)
#link_directories( ${PROJECT_SOURCE_DIR}/external/spdlog/build)
add_library(core STATIC
	  "WasabiEngine.cpp" "WindowBuilder.cpp" "SFMLWindow.cpp" "include/core/EventsStack.hpp" "include/core/Event.hpp" "include/core/EventVisitor.hpp" "include/core/MouseClickEvent.hpp"  "EventsStack.cpp" "include/core/OnCloseEvent.hpp" "../include/Actor.hpp" "details/Container.cpp")

target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/src/include PRIVATE ${GLM_INCLUDE_DIRS} ${RENDERING_INCLUDES} ${COMPONENTS_INCLUDE})

target_link_libraries(core PRIVATE wasabi-core-ecs wasabi-rendering sfml-window-s-d sfml-system-s-d opengl32 winmm gdi32 sfml-graphics-s-d spdlog::spdlog)

