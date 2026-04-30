include(FetchContent)

# imgui
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_SHALLOW TRUE
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

# implot
FetchContent_Declare(implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_SHALLOW TRUE
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/implot
)

FetchContent_MakeAvailable(implot)

add_library(implot
    STATIC
        deps/imgui/imgui.cpp
        deps/implot/implot.cpp
        deps/implot/implot_items.cpp
)

target_include_directories(implot
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui
        ${CMAKE_CURRENT_LIST_DIR}/deps/imgui/backends
        ${CMAKE_CURRENT_LIST_DIR}/deps/implot
)
