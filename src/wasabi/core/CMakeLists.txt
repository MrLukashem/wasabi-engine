
#set(SFML_INCLUDE ${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/include)
#include_directories(wasabi-engine-main ${SFML_INCLUDE})
set(BUILD_SHARED_LIBS OFF)
#link_directories(${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/lib)
#set(SFML_INCLUDE ${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/include)
#link_directories( ${PROJECT_SOURCE_DIR}/external/spdlog/build)
set(CORE_SOURCES
		"WindowBuilder.cpp"
		"GLFWWindow.cpp"
		"include/core/EventsStack.hpp"
		"include/core/Event.hpp" "include/core/EventVisitor.hpp" "include/core/MouseClickEvent.hpp"  "EventsStack.cpp" "include/core/OnCloseEvent.hpp" "details/Container.cpp"
		include/core/MetalLayerHelper.h
		include/core/GLFWKeyEvent.h
)

if (APPLE)
	list(APPEND CORE_SOURCES "MetalLayerHelper.mm")
endif()

if (WIN32)
	list(APPEND CORE_SOURCES "SFMLWindow.cpp")
endif()

add_library(core STATIC ${CORE_SOURCES})

target_include_directories(core PUBLIC include include/core ${COMPONENTS_INCLUDE} ${PLATFORM_INCLUDE} ${UTILS_INCLUDE} PRIVATE ${GLM_INCLUDE_DIRS} ${RENDERING_INCLUDES})

if (WIN32)
	set(SFML_STATIC_LIBRARIES TRUE)
	set(SFML_USE_STATIC_STD_LIBS TRUE)
	set(SFML_STATIC TRUE)
	add_definitions(-DSFML_STATIC)
	target_link_libraries(core PRIVATE
			wasabi-ecs
			sfml-system
			sfml-window
			wasabi-rendering
			opengl32
			winmm
			gdi32
			spdlog::spdlog)
endif()

if (UNIX)
	target_link_libraries(core
		PUBLIC
			wasabi-ecs
			wasabi-rendering
		PRIVATE
			glfw
			spdlog
	)
endif()

