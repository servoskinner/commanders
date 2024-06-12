# Specifies the minimum version of CMake required to process this CMakeLists file.
cmake_minimum_required(VERSION 3.10)

# Sets the name of the project and stores it in the PROJECT_NAME variable.
project(corporatewars)

# Sets the C++ standard to C++11 for the project.
set(CMAKE_CXX_STANDARD 17)
# Ensures that the C++ standard is strictly adhered to and is a requirement.
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Adds the debug flag "-g" to the C++ compiler fl2ags.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Adds the specified directory to the list of paths in which the compiler should search for include files.
include_directories(GameLogic/include)

# Gathers a list of source files matching the specified glob pattern and assigns it to the variable SOURCES.
file(GLOB SOURCES "GameLogic/source/*.cpp")

# Looks for and configures the SFML package, requiring at least version 2.5 and the specified components (system, window, graphics, network, audio).
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Creates executables based on the source files listed in the SOURCES variable.
add_executable(src ${SOURCES})

add_executable(cli_test ${SOURCES} GameLogic/Test_main.cpp)
add_executable(ncurses_test GameLogic/source/NCurses_commander.cpp GameLogic/source/Misc_functions.cpp 
               GameLogic/source/Description_generator.cpp GameLogic/Ncurses_test.cpp)
add_executable(socket_test_p2p GameLogic/Socket_test_p2p.cpp GameLogic/source/Socket_wrapper.cpp)

# Links the "corporatewars" executable target with the SFML libraries.
target_link_libraries(src sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(ncurses_test PRIVATE ncurses)

# Sets properties for the "test" target, specifically setting the output name of the executable to "test".
set_target_properties(cli_test PROPERTIES OUTPUT_NAME "test")
set_target_properties(socket_test_p2p PROPERTIES OUTPUT_NAME "socket_test_p2p")
set_target_properties(ncurses_test PROPERTIES OUTPUT_NAME "ncurses_test")

# Adds a preprocessor definition called TESTING for the "test" target, which is only visible when compiling this target.
target_compile_definitions(cli_test PRIVATE TESTING)
target_compile_definitions(socket_test_p2p PRIVATE TESTING)
target_compile_definitions(ncurses_test PRIVATE TESTING)

# Sets the directory where CMake will place all runtime targets (executables) after building, relative to the binary directory of the project.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
