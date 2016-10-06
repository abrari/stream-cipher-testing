cmake_minimum_required(VERSION 3.5)
project(stream_cipher)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES main.c salsa20/salsa20.h salsa20/salsa20.c matrix.h matrix.c bytes.h bytes.c tests_algebraic.h tests_algebraic.c tests_randommapping.h tests_randommapping.c tests_correlation.h tests_correlation.c)
add_executable(stream_cipher ${SOURCE_FILES})
target_link_libraries(stream_cipher m)
