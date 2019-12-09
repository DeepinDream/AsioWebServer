cmake_minimum_required(VERSION 3.5.0)
project(testapp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE test_SRCS *_test.cpp)

include_directories(${ATP_ROOT})
include_directories(${ATP_ROOT}/src)
# include_directories(${ATP_ROOT}/src/mod2)
# include_directories(${ATP_ROOT}/src/timezone)

# file(GLOB_RECURSE HEAD_RIGEL_EDITOR *.h)  #GLOB 获取./include/目录下的*.h 储存在变量HEAD_RIGEL_EDITOR中
# file(GLOB_RECURSE MOD1_SRCS ${ATP_ROOT}/src/mod1/*.cpp)

add_executable(testapp ${test_SRCS} 
# ${MOD1_SRCS}
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