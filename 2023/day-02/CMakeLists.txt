cmake_minimum_required(VERSION 3.5)

project(day-02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(day-02 main.cpp
    input.txt)

include(GNUInstallDirs)
install(TARGETS day-02
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
