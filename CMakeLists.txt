cmake_minimum_required(VERSION 3.21)

project(plague LANGUAGES C)

add_subdirectory(extern/glfw)

add_executable(plague src/main.c)
target_link_libraries(plague glfw)
