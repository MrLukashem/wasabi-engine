
add_subdirectory(core)
add_subdirectory(physics)
add_subdirectory(renderer)

# add the executable
add_library(wasabi-engine-lib
    main.cpp
)
add_executable(wasabi-engine-main
    main.cpp
)
