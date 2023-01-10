

SET(CMAKE_CXX_FLAGS "-std=c++17")

find_package(glm REQUIRED)

add_library(
		wasabi-rendering STATIC
		systems/ShapesRenderingSystem.cpp
		details/VulkanUtils.cpp
		details/linux/VulkanLinuxUtils.cpp
		details/win/VulkanWinUtils.cpp
		details/macos/VulkanMacOsUtils.cpp
		VulkanRenderer.cpp)

include_directories("/opt/homebrew/Cellar/glm/0.9.9.8/include")
target_link_libraries(wasabi-rendering wasabi-utils wasabi-ecs wasabi-platform spdlog Vulkan::Vulkan glm::glm)

target_include_directories(wasabi-rendering PUBLIC include PRIVATE ${COMPONENTS_INCLUDE} ${PLATFORM_INCLUDE} ${GLM_INCLUDE_DIRS})
