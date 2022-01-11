cmake_minimum_required(VERSION 3.10)
project(starfish)

# add_subdirectory (thirdparty/glog)
# add_subdirectory (thirdparty/googletest)
# add_subdirectory (thirdparty/benchmark)

set (CMAKE_CXX_STANDARD 17)

add_executable(starfish src/main.cpp src/board.cpp src/move.cpp src/piece.cpp src/square.cpp src/utils.cpp)
# target_link_libraries (starfish glog::glog)
# target_link_libraries(starfish benchmark::benchmark)
