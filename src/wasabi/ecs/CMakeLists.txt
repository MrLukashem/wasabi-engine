

add_library(wasabi-ecs STATIC WorldSupervisor.cpp)

set(PRIVATE_INCLUDES
	include/ecs/detail)

set(PUBLIC_INCLUDES
	include)

target_include_directories(wasabi-ecs PUBLIC ${PUBLIC_INCLUDES})
