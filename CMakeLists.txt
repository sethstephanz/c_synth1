cmake_minimum_required(VERSION 3.15)

project(synth C)

set(CMAKE_C_STANDARD 17)

add_executable(synth src/main.c)

find_package(raylib REQUIRED)

target_link_libraries(synth raylib)
