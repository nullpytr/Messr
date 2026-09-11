cmake_minimum_required(VERSION 3.20)
project(Messr C)

add_executable(rdmsr rdmsr.c)
add_executable(wrmsr wrmsr.c)

foreach(target rdmsr wrmsr)
    target_include_directories(${target} BEFORE PRIVATE
        include/compat   # windows compat headers
        include          # wMSR
    )
    target_compile_options(${target} PRIVATE -std=gnu99 -Wall -Wno-unknown-pragmas -Wno-unused-function)
endforeach()
