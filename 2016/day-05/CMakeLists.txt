cmake_minimum_required(VERSION 3.8)

project(day-05 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(day-05 main.cpp)

target_link_libraries(day-05
    crypto
)
