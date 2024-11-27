CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(jammy_webserver VERSION 1.0)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
configure_file("config.h.in" "config.h")
include_directories(BEFORE 
    "include/echo"
    "include/network"
    "include/http"
    "include/buffer"
    "include/log"
    "include/sqlpool"
    "build"
)
#clang编译器添加
#add_compile_options(-fstandalone-debug)
add_subdirectory(src)
