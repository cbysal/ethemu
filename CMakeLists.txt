cmake_minimum_required(VERSION 3.24)
project(ethemu)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(include/leveldb)

include_directories(include)
include_directories(include/leveldb/include)

file(GLOB_RECURSE DIR_SRC "src/*.cpp")

add_executable(ethemu ${DIR_SRC})

target_link_libraries(ethemu leveldb)
