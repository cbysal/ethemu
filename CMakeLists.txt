cmake_minimum_required(VERSION 3.24)
project(ethemu)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB_RECURSE DIR_SRC "src/*.cpp")

add_executable(ethemu ${DIR_SRC})
