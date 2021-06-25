

find_package(glm REQUIRED)

add_library(wasabi-rendering STATIC
	systems/ShapesRenderingSystem.cpp
	details/VulkanUtils.cpp
	VulkanRenderer.cpp)

target_link_libraries(wasabi-rendering wasabi-utils wasabi-ecs wasabi-platform spdlog::spdlog Vulkan::Vulkan glm::glm)

target_include_directories(wasabi-rendering PUBLIC include PRIVATE ${COMPONENTS_INCLUDE} ${PLATFORM_INCLUDE})
