

find_package(glm REQUIRED)

add_library(wasabi-rendering STATIC
	systems/ShapesRenderingSystem.cpp
	VulkanRenderer.cpp)

target_link_libraries(wasabi-rendering wasabi-ecs spdlog::spdlog Vulkan::Vulkan glm::glm)

target_include_directories(wasabi-rendering PUBLIC include PRIVATE ${COMPONENTS_INCLUDE} ${PLATFORM_INCLUDE})
