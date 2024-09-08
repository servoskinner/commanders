#pragma once

#include "TUI.hpp"
#include "Storage_manager.hpp"
#include "Serialization.hpp"

#include <vector>
#include <string>
#include <optional>

bool save_sprite(TUI::Sprite& sprite, std::string filename);
std::optional<TUI::Sprite> load_sprite(std::string filename);