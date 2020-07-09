cmake_minimum_required(VERSION 3.14)
project(fluid)
set(CMAKE_CXX_FLAGS "-g")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(main src/main.cpp src/fluid.cpp)

target_link_libraries(main glfw OpenGL::GL)