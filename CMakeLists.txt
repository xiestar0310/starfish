cmake_minimum_required(VERSION 3.10)
project(starfish)

add_subdirectory (thirdparty/glog)

add_executable(starfish src/main.cpp)
target_link_libraries (starfish glog::glog)
