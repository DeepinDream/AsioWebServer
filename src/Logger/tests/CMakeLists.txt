cmake_minimum_required(VERSION 3.10)

# 开启 debug 单步调试
add_definitions("-Wall -g")

add_executable(LoggingTest LoggingTest.cpp)
target_link_libraries(LoggingTest libLogger)