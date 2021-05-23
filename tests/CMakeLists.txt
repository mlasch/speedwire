cmake_minimum_required(VERSION 3.16)

project(speedwire_tests C)
include(../speedwire/speedwire.cmake)
include(../inserter/inserter.cmake)
include_directories(${SPEEDWIRE_HEADERS_DIR})
include_directories(${INSERTER_HEADERS_DIR})
enable_testing()
add_executable(${PROJECT_NAME} ${SPEEDWIRE_SOURCES} ${INSERTER_SOURCES} tests.c captured_packets.c)
target_link_libraries(${PROJECT_NAME} PRIVATE cmocka)

add_test(NAME unittest COMMAND ${PROJECT_NAME})
