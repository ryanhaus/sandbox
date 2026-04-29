include(FetchContent)

# imgui
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

target_include_directories(imgui
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui/backends
)
