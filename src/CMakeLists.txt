cmake_minimum_required(VERSION 3.5.0)
project(app VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 开启 debug 单步调试
add_definitions("-Wall -g")

file(GLOB_RECURSE ALL_SRCS ./*.cpp ./*.h)

file(GLOB app_SRCS "main.cpp")

include_directories(./)

add_executable(app 
${app_SRCS}
${ALL_SRCS}
)

# 打开断点
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")  
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_definitions(-DASIO_STANDALONE)

# add ssl
find_package(OpenSSL REQUIRED)
target_link_libraries(app OpenSSL::SSL)

# add logger
include_directories(${ATP_ROOT}/common)
target_link_libraries(app libLogger)
