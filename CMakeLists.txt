cmake_minimum_required(VERSION 3.20)
project(Messr C)

add_executable(rdmsr rdmsr.c)
add_executable(wrmsr wrmsr.c)

foreach(target rdmsr wrmsr)
    target_include_directories(${target} BEFORE PRIVATE
        include/compat   # windows compat headers
        include          # wMSR
    )
    if(MSVC)
        target_compile_options(${target} PRIVATE /W3 /wd4068 /wd4505)
    else()
        target_compile_options(${target} PRIVATE -std=gnu99 -Wall -Wno-unknown-pragmas -Wno-unused-function -Wno-uninitialized)
    endif()
    if(WIN32 AND NOT MSVC)
        target_link_options(${target} PRIVATE -s)
    endif()
endforeach()
