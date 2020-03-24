cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(LIB_SRC
    AsyncLogger.cpp
    AsyncLogger2.cpp
    # FileUtil.cpp
    fileutil_win32.cpp
    LogFile.cpp
    Logger.cpp
    LogStream.cpp
    LogFileSlicer.cpp
)

link_libraries(stdc++fs)
add_library(libLogger ${LIB_SRC})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set_target_properties(libLogger PROPERTIES OUTPUT_NAME "Logger")

add_subdirectory(tests)
