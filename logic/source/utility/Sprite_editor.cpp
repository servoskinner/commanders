#include "Storage_manager.hpp"
#include "TUI.hpp"
#include "Event_timer.hpp"
#include "Serialization.hpp"
#include "Animations.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <utility>

inline int flatten_index(std::pair<int, int> indices, int width)
{
    return indices.first * width + indices.second;
}

int main()
{
    std::string filename;
    std::cout << "Enter filename: " << std::flush;
    std::cin >> filename;

    Storage_manager storage(filename);
    TUI& tui = tui.get();
    
    Blinker cursor;
    TUI::Rect cursor_rect; 
    TUI::Sprite sprite(1, 1);

    int width, height;

    if (!storage.get_item("width").has_value()  || 
        !storage.get_item("height").has_value() ||
        !storage.get_item("sprite").has_value()) {

        width, height;
        std::cout << "Creating new file. Enter sprite dimensions:\nWidth: " << std::flush;
        std::cin >> width;
        std::cout << "Height: " << std::flush;
        std::cin >> height;

        TUI::Sprite new_sprite(height, width);
        sprite = new_sprite;

        storage.put_item("width", serialize<int>(width));
        storage.put_item("height", serialize<int>(height));
        storage.put_item("sprite", serialize_vector<TUI::Glyph>(std::vector<TUI::Glyph>(width * height, {' ', 0})));
    }
    else {
        std::vector<TUI::Glyph> glyph_map = deserialize_vector<TUI::Glyph>(storage.get_item("sprite").value());

        width = deserialize<int>(storage.get_item("width").value());
        height = deserialize<int>(storage.get_item("height").value());

        TUI::Sprite new_sprite(height, width);
        sprite = new_sprite;

        for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                sprite.set_glyph(y, x, glyph_map[flatten_index({y, x}, width)]);
            }
        }
    }
}