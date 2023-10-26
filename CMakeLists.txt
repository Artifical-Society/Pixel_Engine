cmake_minimum_required(VERSION 3.26)
project(Pixel_Engine)

set(CMAKE_CXX_STANDARD 17)

add_executable(Pixel_Engine src/main.cpp
        src/utility/decimal.cpp
        src/utility/decimal.h
        src/utility/integer.cpp)
