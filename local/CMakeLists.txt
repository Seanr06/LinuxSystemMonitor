cmake_minimum_required(VERSION 3.13)

project(LinuxMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Threads REQUIRED)

include_directories(include)

add_executable(monitor
    src/main.cpp
    src/stat.cpp
    src/ui_ncurses.cpp
    src/mem.cpp
    src/systeminfo.cpp
    src/process.cpp
    src/collector.cpp
    src/json_serializer.cpp
    src/server_uploader.cpp
)

target_compile_options(monitor PRIVATE -Wall -Wextra)

target_link_libraries(monitor PRIVATE ncursesw Threads::Threads)