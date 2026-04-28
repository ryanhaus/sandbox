cmake_minimum_required(VERSION 3.23)
project(sdl-test)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# libraries
include(FetchContent)
find_package(SDL3 REQUIRED)

FetchContent_Declare(imgui
    URL https://github.com/ocornut/imgui/archive/docking.zip
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui
)

FetchContent_MakeAvailable(imgui)

add_library(imgui
    STATIC
        deps/imgui/imgui.cpp
        deps/imgui/imgui_draw.cpp
        deps/imgui/imgui_tables.cpp
        deps/imgui/imgui_widgets.cpp

        deps/imgui/backends/imgui_impl_sdl3.cpp
        deps/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui/backends
)

# executable
add_executable(sdl-test
    src/main.cpp
)

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui/backends
)

target_link_libraries(sdl-test
    PRIVATE
        SDL3::SDL3
        imgui
)

