

add_library(wasabi-core-ecs WorldSupervisor.cpp)

set(PRIVATE_INCLUDES
	include/ecs/detail)

set(PUBLIC_INCLUDES
	include)

target_include_directories(wasabi-core-ecs PUBLIC ${PUBLIC_INCLUDES} PRIVATE ${PRIVATE_INCLUDES})
