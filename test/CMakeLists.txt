cmake_minimum_required(VERSION 3.5.0)
project(testapp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE test_SRCS *_test.cpp)

include_directories(${ATP_ROOT})
include_directories(${ATP_ROOT}/src)

file(GLOB_RECURSE SRC_SRCS ${ATP_ROOT}/src/*.cpp ${ATP_ROOT}/src/*.h)
list(REMOVE_ITEM SRC_SRCS ${ATP_ROOT}/src/main.cpp) # 排除 src 文件夹的 main.cpp

add_executable(testapp 
${test_SRCS} 
${SRC_SRCS}
)

# 添加库
# add_subdirectory(${ATP_ROOT}/src/mod2)
# target_link_libraries(testapp Mod2)

# 添加库
# add_subdirectory(${ATP_ROOT}/src/timezone)
# target_link_libraries(testapp TimeZone)

add_subdirectory(googletest-master GTest)
include_directories(googletest-master/googlemock/include)
include_directories(googletest-master/googletest/include)
target_link_libraries(testapp gmock)
target_link_libraries(testapp gtest)

# add logger
include_directories(${ATP_ROOT}/common)
target_link_libraries(testapp libLogger)