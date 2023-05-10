cmake_minimum_required(VERSION 3.10)
project(corporatewars)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(gameLogic/include)

file(GLOB SOURCES "gameLogic/source/*.cpp")

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

add_executable(corporatewars ${SOURCES})
add_executable(test ${SOURCES} gameLogic/source/TerminalControl.cpp gameLogic/source/Test_main.cpp)

target_link_libraries(corporatewars sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(test sfml-system sfml-window sfml-graphics sfml-network sfml-audio)

set_target_properties(test PROPERTIES OUTPUT_NAME "test")

target_compile_definitions(test PRIVATE TESTING)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)