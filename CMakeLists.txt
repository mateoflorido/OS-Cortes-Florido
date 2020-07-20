cmake_minimum_required(VERSION 2.4)

project(PipeTwitter C)
find_package(Threads REQUIRED)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-ansi -pedantic")

add_executable(gestor Server.c Server.h Request.h Tweet.h Response.h)
add_executable(cliente Client.c Client.h Request.h Tweet.h Response.h)
target_link_libraries(gestor ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(cliente ${CMAKE_THREAD_LIBS_INIT})