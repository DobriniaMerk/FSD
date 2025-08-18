cmake_minimum_required(VERSION 3.10)
project(FSD CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(FSDWrite FSTWrite/Main.cpp FSTWrite/Images.cpp FSTWrite/Files.cpp FSTWrite/libzpaq.cpp)
include_directories(imgui imgui/backends tinyfiledialogs/ ${SDL2_INCLUDE_DIRS})

add_library(tinyfiledialogs tinyfiledialogs/tinyfiledialogs.c)

add_library(imgui imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_sdlrenderer2.cpp)

target_link_libraries(FSDWrite PRIVATE tinyfiledialogs imgui ${SDL2_LIBRARIES} SDL2_image::SDL2_image)
