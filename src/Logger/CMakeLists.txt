set(LIB_SRC
    AsyncLogger.cpp
    FileUtil.cpp
    LogFile.cpp
    Logger.cpp
    LogStream.cpp
)

add_library(libLogger ${LIB_SRC})

set_target_properties(libLogger PROPERTIES OUTPUT_NAME "Logger")

add_subdirectory(tests)