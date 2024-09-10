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
# find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Creates executables based on the source files listed in the SOURCES variable.
add_executable(game_test logic/source/gameplay/Ability.cpp logic/source/gameplay/Ability.cpp
               logic/source/gameplay/Card.cpp logic/source/gameplay/Deck.cpp logic/source/gameplay/Effects.cpp logic/source/gameplay/Game_master.cpp
               logic/source/gameplay/Player.cpp logic/source/gameplay/Tile.cpp logic/source/interface/Description_generator.cpp 
               logic/source/utility/Unique.cpp logic/source/utility/Misc_functions.cpp logic/source/interface/NCurses_commander.cpp 
               logic/source/utility/Focus.cpp logic/source/utility/TUI.cpp main/Ncurses_test.cpp)

add_executable(socket_test_p2p logic/source/network/UDP_wrapper.cpp main/Socket_test_p2p.cpp )
add_executable(tcp_client logic/source/network/TCP_client.cpp main/TCP_test_client.cpp)
add_executable(tcp_server logic/source/network/Socket_wrappers.cpp logic/source/network/TCP_server.cpp main/TCP_test_server.cpp)

add_executable(timer_test logic/source/utility/Event_timer.cpp main/Timer_test.cpp)
add_executable(unique_test logic/source/utility/Unique.cpp main/Unique_test.cpp)
add_executable(storage_test logic/source/utility/Storage_manager.cpp main/Storage_test.cpp)

add_executable(tui_test logic/source/utility/TUI.cpp logic/source/utility/Focus.cpp
               logic/source/utility/Misc_functions.cpp logic/source/utility/Animations.cpp logic/source/utility/Event_timer.cpp 
               logic/source/utility/Storage_manager.cpp logic/source/utility/Sprite_storage.cpp main/TUI_test.cpp)

add_executable(generic_client_test logic/source/network/TCP_client.cpp logic/source/network/UDP_wrapper.cpp 
               logic/source/network/Socket_wrappers.cpp main/Generic_client_test.cpp)
add_executable(generic_server_test logic/source/network/TCP_server.cpp logic/source/network/UDP_wrapper.cpp 
               logic/source/network/Socket_wrappers.cpp main/Generic_server_test.cpp)

add_executable(sprite_editor logic/source/utility/Sprite_editor.cpp logic/source/utility/TUI.cpp logic/source/utility/Misc_functions.cpp  
               logic/source/utility/Animations.cpp logic/source/utility/Event_timer.cpp logic/source/utility/Sprite_storage.cpp
               logic/source/utility/Storage_manager.cpp)

add_executable(game main/Game.cpp logic/source/utility/TUI.cpp logic/source/utility/Misc_functions.cpp  
               logic/source/utility/Animations.cpp logic/source/utility/Event_timer.cpp logic/source/utility/Sprite_storage.cpp
               logic/source/utility/Storage_manager.cpp)

# Links the "corporatewars" executable target with the SFML libraries.
# target_link_libraries(game sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(game_test PRIVATE ncurses)
target_link_libraries(tui_test PRIVATE ncurses)
target_link_libraries(sprite_editor PRIVATE ncurses)
target_link_libraries(game PRIVATE ncurses)

# Adds a preprocessor definition called TESTING for the "test" target, which is only visible when compiling this target.
target_compile_definitions(game_test PRIVATE TESTING)
target_compile_definitions(socket_test_p2p PRIVATE TESTING)

# Sets the directory where CMake will place all runtime targets (executables) after building, relative to the binary directory of the project.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
