cmake_minimum_required(VERSION 3.10)
project(clog C)

add_library(clog clog.c)
target_include_directories(clog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT MSVC)
    target_compile_options(clog PRIVATE -Wall -Wextra -pedantic)
    target_compile_features(clog PRIVATE c_std_99)
endif()

add_executable(clog_test test_clog.c)
target_link_libraries(clog_test PRIVATE clog)

enable_testing()
add_test(NAME selftest COMMAND clog_test)
