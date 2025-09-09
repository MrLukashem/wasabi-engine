
add_library(wasabi-platform
		Config.cpp
		AppleNativeBridge.mm
)

find_library(COCOA_LIBRARY Cocoa)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(METAL_LIBRARY Metal)


target_link_libraries(wasabi-platform
		PRIVATE
		${COCOA_LIBRARY}
		${QUARTZCORE_LIBRARY}
		${METAL_LIBRARY}
)

target_include_directories(wasabi-platform PUBLIC include)

