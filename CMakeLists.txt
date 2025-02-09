# Specifies the minimum version of CMake required to process this CMakeLists file.
cmake_minimum_required(VERSION 3.10)

# Sets the name of the project and stores it in the PROJECT_NAME variable.
project(corporatewars)

# Sets the C++ standard to C++11 for the project.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# Adds the specified directory to the list of paths in which the compiler should search for include files.
file (GLOB INCLUDE "logic/*/headers")
include_directories(${INCLUDE})

# Gathers a list of source files matching the specified glob pattern and assigns it to the variable SOURCES.
file(GLOB SOURCES "logic/*/source/*.cpp")

# Looks for and configures the SFML package, requiring at least version 2.5 and the specified components (system, window, graphics, network, audio).
# find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Creates executables based on the source files listed in the SOURCES variable.
add_executable(game_test logic/gameplay/source/Ability.cpp logic/gameplay/source/Ability.cpp
               logic/gameplay/source/Card.cpp logic/gameplay/source/Deck.cpp logic/gameplay/source/Effects.cpp logic/gameplay/source/Game_master.cpp
               logic/gameplay/source/Player.cpp logic/gameplay/source/Tile.cpp logic/interface/source/Description_generator.cpp 
               logic/utility/source/Unique.cpp logic/utility/source/Misc_functions.cpp logic/interface/source/NCurses_commander.cpp 
               logic/utility/source/Focus.cpp logic/tui/source/TUI.cpp logic/tui/source/NCursesTUI.cpp logic/tui/source/Rect.cpp
               logic/tui/source/Scrollable_text.cpp logic/tui/source/Sprite.cpp logic/tui/source/Text_input.cpp logic/tui/source/Text.cpp
               logic/tui/source/UI_Object.cpp logic/utility/source/Logger.cpp main/Ncurses_test.cpp)

add_executable(tui_test logic/utility/source/Misc_functions.cpp logic/utility/source/Focus.cpp 
               logic/tui/source/TUI.cpp logic/tui/source/NCursesTUI.cpp logic/tui/source/Rect.cpp 
               logic/tui/source/Scrollable_text.cpp logic/tui/source/Sprite.cpp logic/tui/source/Text_input.cpp 
               logic/tui/source/Text.cpp logic/tui/source/UI_Object.cpp logic/utility/source/Animations.cpp logic/utility/source/Timer.cpp 
               main/TUI_test.cpp)

# target_link_libraries(game sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_link_libraries(game_test PRIVATE ncurses)
target_link_libraries(tui_test PRIVATE ncurses)

target_compile_definitions(game_test PRIVATE LOGGER_ON)

# Sets the directory where CMake will place all runtime targets (executables) after building, relative to the binary directory of the project.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
