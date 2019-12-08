cmake_minimum_required(VERSION 3.5.0)
project(app VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 开启 debug 单步调试
add_definitions("-Wall -g")

file(GLOB_RECURSE HEAD_RIGEL_EDITOR *.h)  #GLOB 获取./include/目录下的*.h 储存在变量HEAD_RIGEL_EDITOR中
file(GLOB_RECURSE WEBSERVER_SRC ./WebServer/*.cpp ./WebServer/*.h)

file(GLOB app_SRCS "main.cpp")

add_executable(app ${HEAD_RIGEL_EDITOR} ${WEBSERVER_SRC} ${app_SRCS})

# 打开断点
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")  
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_definitions(-DASIO_STANDALONE)

# add ssl
find_package(OpenSSL REQUIRED)
target_link_libraries(app OpenSSL::SSL)
