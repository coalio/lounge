cmake_minimum_required(VERSION 3.20)

project(lounge LANGUAGES CXX)

# force C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# find SDL2 (config preferred, fallback to module)
find_package(SDL2 QUIET CONFIG)
if(NOT SDL2_FOUND)
    find_package(SDL2 REQUIRED)
endif()

# source files
set(SOURCES
    main.cpp
    engine/platform/sdl_platform.cpp
    engine/input/input_handler.cpp
    engine/resources/resource_manager.cpp
    engine/render/renderer.cpp
    game/render/scene_renderer.cpp
    game/pipeline/game_pipeline.cpp
    game/game.cpp
)

# executable
add_executable(lounge ${SOURCES})

# include directories for your headers only
target_include_directories(lounge
    PRIVATE ${CMAKE_SOURCE_DIR}
)

# allow plain main() for SDL
target_compile_definitions(lounge PRIVATE SDL_MAIN_HANDLED)

# link SDL2
if(TARGET SDL2::SDL2)
    target_link_libraries(lounge PRIVATE SDL2::SDL2)
elseif(SDL2_LIBRARIES)
    target_include_directories(lounge PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(lounge PRIVATE ${SDL2_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2 not found or unsupported CMake package configuration.")
endif()
