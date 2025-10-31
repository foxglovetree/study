
message(STATUS "Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.10)

project(study_ogre CXX)


find_package(fmt REQUIRED)
find_package(OGRE REQUIRED COMPONENTS Bites CONFIG)

add_executable(Study_Ogre main.cpp)

target_link_libraries(Study_Ogre PRIVATE fmt::fmt OgreBites)
