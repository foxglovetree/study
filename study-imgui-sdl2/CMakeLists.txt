cmake_minimum_required(VERSION 3.16)
set(PRJECT_NAME Study_Imgui)
project(${PRJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Ogre
find_package(OpenGL REQUIRED)
find_package(sdl2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

file(GLOB SOURCES "src/*.cpp")
add_compile_definitions(UNICODE _UNICODE)
# 添加可执行文件
add_executable(${PRJECT_NAME} ${SOURCES})

# 链接Ogre库
target_link_libraries(${PRJECT_NAME} PRIVATE
    imgui::imgui
    OpenGL::GL
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )
# 包含头文件
target_include_directories(${PRJECT_NAME} PRIVATE include)

target_compile_options(${PRJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-finput-charset=UTF-8 -fexec-charset=UTF-8>
)