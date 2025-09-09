

SET(CMAKE_CXX_FLAGS "-std=c++20")

find_package(glm REQUIRED)
find_package(vulkan REQUIRED)

add_library(
		wasabi-rendering STATIC
		systems/ShapesRenderingSystem.cpp
		details/VulkanUtils.cpp
		details/linux/VulkanLinuxUtils.cpp
		details/win/VulkanWinUtils.cpp
		details/macos/VulkanMacOsUtils.cpp
		VulkanRenderer.cpp
		include/rendering/Shader.h)

include_directories("/opt/homebrew/Cellar/glm/0.9.9.8/include")

target_link_libraries(wasabi-rendering
		PUBLIC
			volk
			vulkan
			glm::glm
		PRIVATE
			wasabi-utils
			wasabi-ecs
			wasabi-platform
			spdlog
)

target_include_directories(wasabi-rendering PUBLIC include PRIVATE ${COMPONENTS_INCLUDE} ${PLATFORM_INCLUDE} ${GLM_INCLUDE_DIRS})
