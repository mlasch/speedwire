cmake_minimum_required(VERSION 3.16)
project(speedwire C)

set(CMAKE_C_STANDARD 11)

include(../speedwire/speedwire.cmake)
include(../inserter/inserter.cmake)
include_directories(${SPEEDWIRE_HEADERS_DIR})
include_directories(${INSERTER_HEADERS_DIR})

add_executable(speedwire ${SPEEDWIRE_SOURCES} ${INSERTER_SOURCES} main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)