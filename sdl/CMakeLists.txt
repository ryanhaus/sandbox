cmake_minimum_required(VERSION 3.23)
project(sdl-test)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(fetch_dependencies.cmake)
find_package(SDL3 REQUIRED)

add_executable(sdl-test
    src/main.cpp
)

target_link_libraries(sdl-test
    PRIVATE
        SDL3::SDL3
        imgui
)

