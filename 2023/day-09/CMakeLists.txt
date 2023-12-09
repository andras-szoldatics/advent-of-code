cmake_minimum_required(VERSION 3.5)

project(day-09 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(day-09 main.cpp
    input.txt)

include(GNUInstallDirs)
install(TARGETS day-09
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
