#include "Storage_manager.hpp"
#include "TUI.hpp"
#include "Event_timer.hpp"
#include "Serialization.hpp"
#include "Animations.hpp"
#include "Misc_functions.hpp"
#include "Sprite_storage.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <utility>
#include <cstdlib>

#define MAX_SPRITE_SIZE 48
#define COLOR_SELECTOR_OFFSET_X 12
#define COLOR_SELECTOR_OFFSET_Y MAX_SPRITE_SIZE + 4

#define CHAR_SELECTOR_WIDTH 12


int main(int argc, char* argv[])
{
    if (argc < 2) {
        throw std::runtime_error("No filename provided");
    }

    // Initialize file
    TUI::Sprite sprite(1, 1);
    sprite.x = 2;
    sprite.y = 2;

    int width, height;
    std::optional<TUI::Sprite> loaded = load_sprite(argv[1]);

    if(!loaded.has_value())
    {
        Storage_manager storage_manager(argv[1]);

        if (argc < 4) {
            throw std::runtime_error("No width and height provided");
        }
        width = std::atoi(argv[2]);
        height = std::atoi(argv[3]);

        storage_manager.put("width", width);
        storage_manager.put("height", height);
        storage_manager.put_vector("sprite", std::vector<TUI::Glyph>(width * height, {' '}));
        
        loaded.emplace(width, height);
    }
    else {
        height = loaded.value().get_size().first;
        width = loaded.value().get_size().second;
    }

    sprite = loaded.value();

    // Initialize UI elements
    TUI& tui = tui.get();
    
    Blinker cursor;
    TUI::Rect cursor_rect_on; 
    TUI::Rect cursor_rect_off;
    cursor.children = {std::ref(cursor_rect_on), std::ref(cursor_rect_off)};
    cursor_rect_off.visible = false;

    TUI::Text file_name_indicator;
    file_name_indicator.text = argv[1];

    file_name_indicator.foreground = COLOR_BRIGHT_BLACK;
    file_name_indicator.x = 2;
    file_name_indicator.y = 0;

    cursor_rect_on.width = 1;
    cursor_rect_on.height = 1;
    cursor_rect_off.width = 1;
    cursor_rect_off.height = 1;
    cursor_rect_on.set_all({'+', COLOR_BLACK, COLOR_WHITE});
    cursor_rect_off.set_all({'+', COLOR_WHITE, COLOR_BLACK});

    TUI::Rect drawing_area_box;
    TUI::Rect char_selector_box;
    TUI::Rect drawing_limits_box;

    drawing_area_box.width = MAX_SPRITE_SIZE + 2;
    drawing_area_box.height = MAX_SPRITE_SIZE + 2;
    drawing_area_box.set_border_color(COLOR_BRIGHT_BLACK);
    drawing_area_box.x = 1;
    drawing_area_box.y = 1;

    char_selector_box.width = CHAR_SELECTOR_WIDTH + 2;
    char_selector_box.x = MAX_SPRITE_SIZE + 4;
    char_selector_box.y = 1;
    char_selector_box.height = 18;

    drawing_limits_box.set_all({'.', COLOR_BRIGHT_BLACK});
    drawing_limits_box.x = 1;
    drawing_limits_box.y = 1;

    TUI::Rect brush_box;

    brush_box.width = 3;
    brush_box.height = 3;
    brush_box.x = 2;
    brush_box.y = MAX_SPRITE_SIZE + 4;
    brush_box.set_border_color(COLOR_BRIGHT_BLACK);

    TUI::Text brush_box_text;
    brush_box_text.text = "BRUSH";
    brush_box_text.foreground = COLOR_BRIGHT_BLACK;
    brush_box_text.x = 5;
    brush_box_text.y = MAX_SPRITE_SIZE + 5;

    drawing_limits_box.width = width + 2;
    drawing_limits_box.height = height + 2;

    TUI::Glyph brush;
    int cursor_x = 0, cursor_y = 0;
    unsigned short background, foreground;
    int current_char = 0;
    
    std::vector<unsigned> chars = {' ', ACS_VLINE, ACS_HLINE, ACS_URCORNER, ACS_ULCORNER, ACS_LRCORNER, ACS_LLCORNER, \
                                        ACS_TTEE, ACS_RTEE, ACS_BTEE, ACS_LTEE, ACS_PLUS, ACS_BLOCK, ACS_BULLET, ACS_DIAMOND};
    for(unsigned ch = 33; ch < 127; ch++) {
        chars.push_back(ch);
    }

    // Application loop
    bool is_running = true;
    while(is_running) {
        // Process input
        unsigned input = tui.get_input();

        switch (input)
        {
        case KEY_ENTR:

            break;
        case '`': case '~':
            save_sprite(sprite, argv[1]);
            is_running = false;
            break;
        
        case 'w': case 'W':
            if (cursor_y != 0) {
                cursor_y--;
            }
            break;
        case 'a': case 'A':
            if (cursor_x != 0) {
                cursor_x--;
            }
            break;

        case 's': case 'S':
            if (cursor_y != height-1) {
                cursor_y++;
            }
            break;

        case 'd': case 'D':
        if (cursor_x != width-1) {
                cursor_x++;
            }
            break; 

        case 'q': case 'Q':
            foreground = (foreground - 1)%16;
            break;

        case 'e': case 'E':
            foreground = (foreground + 1)%16;
            break;

        case 'x': case 'X':
            background = (background - 1)%16;
            break;

        case 'c': case 'C':
            background = (background + 1)%16;
            break;

        case 'i': case 'I':
            if (current_char - CHAR_SELECTOR_WIDTH >= 0) {
                current_char -= CHAR_SELECTOR_WIDTH;
            }
            break;
        
        case 'j': case 'J':
            if (current_char != 0) {
                current_char--;
            }
            break;

        case 'k': case 'K':
            if (current_char + CHAR_SELECTOR_WIDTH < chars.size()) {
                current_char += CHAR_SELECTOR_WIDTH;
            }
            break;
        
        case 'l': case 'L':
            if (current_char != chars.size()-1) {
                current_char++;
            }
            break;

        case ' ':
            {
                sprite.set_glyph(cursor_y, cursor_x, brush);
            }
            break ;
        }

        // Render TUI
        tui.clear();

        file_name_indicator.draw();
        drawing_limits_box.draw();
        drawing_area_box.draw();
        char_selector_box.draw();
        sprite.draw();
        brush_box.draw();
        brush_box_text.draw();

        // char selector
        for(int i = 0; i < chars.size(); i++) {
            std::pair<int, int> pos = unflatten_index(i, CHAR_SELECTOR_WIDTH);
            if (i != current_char) {
                tui.draw_glyph(2 + pos.first, MAX_SPRITE_SIZE + 5 + pos.second, {chars[i], COLOR_WHITE, COLOR_BLACK});
            }
            else {
                tui.draw_glyph(2 + pos.first, MAX_SPRITE_SIZE + 5 + pos.second, {chars[i], COLOR_BLACK, COLOR_BRIGHT_WHITE});
            }
        }

        brush.background = background;
        brush.foreground = foreground;
        brush.symbol = chars[current_char];
        tui.draw_glyph(brush_box.y+1, brush_box.x+1, brush);

        cursor.draw(0, cursor_y + 2, cursor_x + 2);

        // color selector
        for (int c = 1; c < 16; c++) {
            tui.draw_glyph(COLOR_SELECTOR_OFFSET_Y, COLOR_SELECTOR_OFFSET_X + 1 + c, {' ', COLOR_BLACK, (unsigned short)c});
            tui.draw_glyph(COLOR_SELECTOR_OFFSET_Y + 2, COLOR_SELECTOR_OFFSET_X + 1 + c, {' ', COLOR_BLACK, (unsigned short)c});
        }
        tui.draw_glyph(COLOR_SELECTOR_OFFSET_Y, COLOR_SELECTOR_OFFSET_X + 1, {'.', COLOR_BRIGHT_BLACK, COLOR_BLACK});
        tui.draw_glyph(COLOR_SELECTOR_OFFSET_Y + 2, COLOR_SELECTOR_OFFSET_X + 1, {'.', COLOR_BRIGHT_BLACK, COLOR_BLACK});

        tui.draw_glyph(COLOR_SELECTOR_OFFSET_Y - 1, COLOR_SELECTOR_OFFSET_X + 1 + foreground, {'v', COLOR_BRIGHT_WHITE, COLOR_BLACK});
        tui.draw_glyph(COLOR_SELECTOR_OFFSET_Y + 3, COLOR_SELECTOR_OFFSET_X + 1 + background, {'^', COLOR_BRIGHT_WHITE, COLOR_BLACK});

        tui.render();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}