

SET(CMAKE_CXX_FLAGS "-std=c++17")

find_package(glm REQUIRED)

add_library(
		wasabi-rendering STATIC
		systems/ShapesRenderingSystem.cpp
		details/VulkanUtils.cpp
		details/linux/VulkanLinuxUtils.cpp
		details/win/VulkanWinUtils.cpp
		VulkanRenderer.cpp)

target_link_libraries(wasabi-rendering wasabi-utils wasabi-ecs wasabi-platform spdlog::spdlog Vulkan::Vulkan glm::glm)

target_include_directories(wasabi-rendering PUBLIC include PRIVATE ${COMPONENTS_INCLUDE} ${PLATFORM_INCLUDE})
