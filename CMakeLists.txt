cmake_minimum_required(VERSION 3.5.0)
# project(app VERSION 0.1.0)

message("\n*** Project atp Start ***")

if(NOT DEFINED ATP_ROOT)
    set(ATP_ROOT "${CMAKE_SOURCE_DIR}")
endif()
message("ATP_ROOT: ${ATP_ROOT}")

add_subdirectory(src)

# add_subdirectory(test)

# include(CTest)
# enable_testing()

# add_executable(app main.cpp)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
