cmake_minimum_required(VERSION 3.10)
project(trade)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(trade main.cpp)

target_link_libraries(trade ${Boost_LIBRARIES})
