cmake_minimum_required(VERSION 3.10)

set(LIB_SRC
    AsyncLogger.cpp
    AsyncLogger2.cpp
    FileUtil.cpp
    LogFile.cpp
    Logger.cpp
    LogStream.cpp
)

add_library(libLogger ${LIB_SRC})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set_target_properties(libLogger PROPERTIES OUTPUT_NAME "Logger")

add_subdirectory(tests)