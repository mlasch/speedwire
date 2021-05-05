cmake_minimum_required(VERSION 3.17)
project(speedwire C)

set(CMAKE_C_STANDARD 11)

include(../speedwire/speedwire.cmake)
include_directories(${SPEEDWIRE_HEADERS_DIR})

add_executable(speedwire ${SPEEDWIRE_SOURCES} main.c)
