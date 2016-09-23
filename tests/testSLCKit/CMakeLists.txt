project(testSLCKit)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../../include)
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

set(CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED )
target_link_libraries(${PROJECT_NAME} Qt5::Core SLCKit)
