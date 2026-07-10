
set(PLATFORM_SOURCES Config.cpp)

if (APPLE)
	list(APPEND PLATFORM_SOURCES AppleNativeBridge.mm)
endif()

add_library(wasabi-platform ${PLATFORM_SOURCES})

if (APPLE)
	find_library(COCOA_LIBRARY Cocoa)
	find_library(QUARTZCORE_LIBRARY QuartzCore)
	find_library(METAL_LIBRARY Metal)

	target_link_libraries(wasabi-platform
			PRIVATE
			${COCOA_LIBRARY}
			${QUARTZCORE_LIBRARY}
			${METAL_LIBRARY}
	)
endif()

target_include_directories(wasabi-platform PUBLIC include)

