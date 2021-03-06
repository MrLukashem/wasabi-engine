


include_directories(include/core)
set(SFML_INCLUDE ${PROJECT_SOURCE_DIR}/external/SFML-prebuilt/SFML-2.5.1/include)
include_directories(wasabi-engine-main ${SFML_INCLUDE})

add_library(core STATIC
	Object.cpp  "WasabiEngine.cpp" "Window.cpp")

target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)

