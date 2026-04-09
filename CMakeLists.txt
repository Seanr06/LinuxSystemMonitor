cmake_minimum_required(VERSION 3.10)

project(LinuxMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(monitor
    src/main.cpp
    src/stat.cpp
    src/ui_ncurses.cpp
    src/uptime.cpp
    src/mem.cpp
)

target_link_libraries(monitor ncursesw)