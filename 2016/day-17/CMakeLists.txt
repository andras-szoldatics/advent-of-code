cmake_minimum_required(VERSION 3.5)

project(day-17 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(day-17 main.cpp)

target_link_libraries(day-17 crypto)

include(GNUInstallDirs)
install(TARGETS day-17
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
