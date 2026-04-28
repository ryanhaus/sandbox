cmake_minimum_required(VERSION 3.23)
project(sdl-test)

find_package(SDL3 REQUIRED)

add_executable(sdl-test
    src/main.cpp
)

target_link_libraries(sdl-test
    PRIVATE
        SDL3::SDL3
)

