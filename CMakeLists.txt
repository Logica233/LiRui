cmake_minimum_required(VERSION 3.5)
project(demo)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -O3 -g")

include_directories(${CMAKE_SOURCE_DIR}/include/)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE sources ${CMAKE_SOURCE_DIR}/src/*.cc)

add_executable(demo main.cc ${sources})

target_link_libraries(demo ${OpenCV_LIBS})
