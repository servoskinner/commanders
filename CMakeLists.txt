# Specifies the minimum version of CMake required to process this CMakeLists file.
cmake_minimum_required(VERSION 3.10)

# Sets the name of the project and stores it in the PROJECT_NAME variable.
project(corporatewars)

# Sets the C++ standard to C++11 for the project.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Adds the specified directory to the list of paths in which the compiler should search for include files.
file (GLOB INCLUDE "logic/include/*")
include_directories(${INCLUDE})

# Gathers a list of source files matching the specified glob pattern and assigns it to the variable SOURCES.
file(GLOB SOURCES "logic/source/*/*.cpp")

# Looks for and configures the SFML package, requiring at least version 2.5 and the specified components (system, window, graphics, network, audio).
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Creates executables based on the source files listed in the SOURCES variable.
add_executable(game_test ${SOURCES} main/Gameplay_test.cpp)
add_executable(ncurses_test logic/source/interface/NCurses_commander.cpp logic/source/utility/Misc_functions.cpp 
               logic/source/interface/Description_generator.cpp main/Ncurses_test.cpp)

add_executable(socket_test_p2p logic/source/network/UDP_wrapper.cpp main/Socket_test_p2p.cpp )
add_executable(tcp_client logic/source/network/TCP_client.cpp main/TCP_test_client.cpp)
add_executable(tcp_server logic/source/network/TCP_server.cpp main/TCP_test_server.cpp)

add_executable(timer_test logic/source/utility/Event_timer.cpp main/Timer_test.cpp)

# Links the "corporatewars" executable target with the SFML libraries.
# target_link_libraries(game sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(ncurses_test PRIVATE ncurses)

# Adds a preprocessor definition called TESTING for the "test" target, which is only visible when compiling this target.
target_compile_definitions(game_test PRIVATE TESTING)
target_compile_definitions(socket_test_p2p PRIVATE TESTING)
target_compile_definitions(ncurses_test PRIVATE TESTING)

# Sets the directory where CMake will place all runtime targets (executables) after building, relative to the binary directory of the project.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
