
message(STATUS "Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

cmake_minimum_required(VERSION 3.10)

project(study_ogre CXX)

# 1. 查找 fmt 库
find_package(fmt REQUIRED)

add_executable(Study_Ogre main.cpp)

# 2. 链接 fmt 库到目标
target_link_libraries(Study_Ogre PRIVATE fmt::fmt)
