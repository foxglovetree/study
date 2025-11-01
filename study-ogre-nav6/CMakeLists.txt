cmake_minimum_required(VERSION 3.16)
project(HexNavigation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Ogre
find_package(OGRE REQUIRED)

# 添加可执行文件
add_executable(Study_Ogre main.cpp)

# 链接Ogre库
target_link_libraries(Study_Ogre ${OGRE_LIBRARIES})

# 包含Ogre头文件
target_include_directories(Study_Ogre PRIVATE ${OGRE_INCLUDE_DIRS})

# 如果需要其他资源路径，可以在这里添加
# target_include_directories(Study_Ogre PRIVATE ${PROJECT_SOURCE_DIR}/resources)