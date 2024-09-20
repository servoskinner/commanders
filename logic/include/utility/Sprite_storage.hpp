#pragma once

#include "TUI.hpp"
#include "Archivist.hpp"
#include "Serialization.hpp"
#include "Misc_functions.hpp"

#include <vector>
#include <string>
#include <optional>

/// @brief Writes sprite data into a file.
bool save_sprite(TUI::Sprite& sprite, std::string filename);
/// @brief Creates sprite from a file.
std::optional<TUI::Sprite> load_sprite(std::string filename);