cmake_minimum_required(VERSION 3.16)
project(speedwire C)

set(CMAKE_C_STANDARD 11)

include(../speedwire/speedwire.cmake)
include(../inserter/inserter.cmake)
include_directories(${SPEEDWIRE_HEADERS_DIR})
include_directories(${INSERTER_HEADERS_DIR})

add_executable(speedwire ${SPEEDWIRE_SOURCES} ${INSERTER_SOURCES} main.c)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)
target_link_libraries(${PROJECT_NAME} PRIVATE pthread curl)

if(SANITIZER)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=${SANITIZER} -fno-sanitize-recover=all)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=${SANITIZER} -fno-sanitize-recover=all)
endif()