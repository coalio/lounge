cmake_minimum_required(VERSION 3.20)

project(lounge LANGUAGES CXX)

# for clangd and other tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# default to C++17 so tdlib config matches upstream expectations
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# tdlib configuration (submodule lives in include/td)
option(TD_ENABLE_JNI "Enable JNI (Java) bindings" OFF)
option(TD_ENABLE_DOTNET "Enable .NET bindings" OFF)
option(TD_ENABLE_LTO "Enable Link Time Optimization" OFF)

add_subdirectory(${CMAKE_SOURCE_DIR}/include/td ${CMAKE_BINARY_DIR}/tdlib_build)

# force C++23 for lounge sources
set(CMAKE_CXX_STANDARD 23)

# find SDL2 (config preferred, fallback to module)
find_package(SDL2 QUIET CONFIG)
if(NOT SDL2_FOUND)
    find_package(SDL2 REQUIRED)
endif()

# find RmlUi (via vcpkg or system config package)
find_package(RmlUi REQUIRED)

# source files
set(SOURCES
    main.cpp
    engine/backend/backend.cpp
    engine/backend/backend_event_handlers.cpp
    engine/backend/network_manager.cpp
    engine/backend/telegram/telegram_backend.cpp
    engine/config/config.cpp
    engine/events/event_service.cpp
    engine/platform/sdl_platform.cpp
    engine/input/input_handler.cpp
    engine/resources/resource_manager.cpp
    engine/ui/ui_context.cpp
    engine/ui/ui_document.cpp
    engine/ui/ui_screen_registry.cpp
    engine/ui/ui_system.cpp
    engine/ui/backends/rml/rml_render_interface.cpp
    engine/ui/backends/rml/rml_system_interface.cpp
    engine/ui/backends/rml/rml_ui_backend.cpp
    engine/render/renderer.cpp
    game/render/scene_renderer.cpp
    game/ui/components/base/label_component.cpp
    game/ui/components/specialized/menu_option_component.cpp
    game/ui/components/specialized/option_list_component.cpp
    game/ui/components/specialized/title_component.cpp
    game/ui/screens/join_friend/join_friend_screen.cpp
    game/ui/screens/gameplay/gameplay_screen.cpp
    game/ui/screens/start_menu/start_menu_screen.cpp
    game/ui/ui_service.cpp
    game/pipeline/stages/input_stage.cpp
    game/pipeline/stages/logic_stage.cpp
    game/pipeline/stages/ui_stage.cpp
    game/pipeline/stages/render_stage.cpp
    game/pipeline/game_pipeline.cpp
    game/game.cpp
)

# executable
add_executable(lounge ${SOURCES})

# link UI, backend, and networking deps
target_link_libraries(lounge
    PRIVATE
        RmlUi::RmlUi
        Td::TdStatic
)

# include directories for your headers only
target_include_directories(lounge
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

# allow plain main() for SDL and expose asset root
target_compile_definitions(lounge PRIVATE
    SDL_MAIN_HANDLED
)

# link SDL2
if(TARGET SDL2::SDL2)
    target_link_libraries(lounge PRIVATE SDL2::SDL2)
elseif(SDL2_LIBRARIES)
    target_include_directories(lounge PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(lounge PRIVATE ${SDL2_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2 not found or unsupported CMake package configuration.")
endif()
