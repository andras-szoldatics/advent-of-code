cmake_minimum_required(VERSION 3.8)

project(day-04 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(day-04 main.cpp)

target_link_libraries(day-04
    crypto
)
