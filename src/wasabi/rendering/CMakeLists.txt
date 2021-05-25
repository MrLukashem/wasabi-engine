

find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(Vulkan REQUIRED)

add_library(wasabi-rendering
	systems/ShapesRenderingSystem.cpp
	VulkanRenderer.cpp)

target_link_libraries(wasabi-rendering wasabi-core-ecs spdlog::spdlog Vulkan::Vulkan)

target_include_directories(wasabi-rendering PUBLIC include PRIVATE ${COMPONENTS_INCLUDE} ${GLM_INCLUDE_DIRS})
