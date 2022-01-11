cmake_minimum_required(VERSION 3.10)
project(starfish)

# add_subdirectory (thirdparty/glog)
# add_subdirectory (thirdparty/googletest)
# add_subdirectory (thirdparty/benchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Ofast -flto -Wall -Wextra -pedantic")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Ofast -flto -Wall -Wextra -pedantic")

add_executable(starfish src/main.cpp src/board.cpp src/move.cpp src/piece.cpp src/square.cpp src/utils.cpp)
# target_link_libraries (starfish glog::glog)
# target_link_libraries(starfish benchmark::benchmark)
