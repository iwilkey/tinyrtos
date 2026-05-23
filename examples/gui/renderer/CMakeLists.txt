cmake_minimum_required(VERSION 3.20)

project(tinyrtos_gui_renderer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

find_path(LIBSERIALPORT_INCLUDE_DIR libserialport.h)
find_library(LIBSERIALPORT_LIBRARY serialport)

add_executable(tinyrtos_gui_renderer
    src/main.c
    src/renderer.c
    src/serial.c
    src/parser.c
)

target_include_directories(tinyrtos_gui_renderer PRIVATE
    include
)

target_include_directories(tinyrtos_gui_renderer PRIVATE
    ${LIBSERIALPORT_INCLUDE_DIR}
    src
)

target_link_libraries(tinyrtos_gui_renderer PRIVATE
    SDL2::SDL2
    ${LIBSERIALPORT_LIBRARY}
)
