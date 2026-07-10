

SET(CMAKE_CXX_FLAGS "-std=c++20")


find_package(Vulkan REQUIRED)

add_library(
		wasabi-rendering STATIC
		systems/ShapesRenderingSystem.cpp
		details/VulkanUtils.cpp
		details/linux/VulkanLinuxUtils.cpp
		details/win/VulkanWinUtils.cpp
		details/macos/VulkanMacOsUtils.cpp
		VulkanRenderer.cpp)

target_link_libraries(wasabi-rendering
		PUBLIC
			volk
			Vulkan::Vulkan
			glm::glm
		PRIVATE
			wasabi-utils
			wasabi-ecs
			wasabi-platform
			GPUOpen::VulkanMemoryAllocator
			spdlog
)

target_include_directories(wasabi-rendering
		PUBLIC
		include
		${CMAKE_CURRENT_SOURCE_DIR}/../include
		PRIVATE
		${COMPONENTS_INCLUDE}
		${PLATFORM_INCLUDE}
		${GLM_INCLUDE_DIRS})
