cmake_minimum_required(VERSION 3.10)
project(corporatewars)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(include)

file(GLOB SOURCES "source/*.cpp")

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

add_executable(corporatewars ${SOURCES})

target_link_libraries(corporatewars sfml-system sfml-window sfml-graphics sfml-network sfml-audio)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)