#pragma once

#include "TUI.hpp"
#include "UI_Object.hpp"

/// @brief Used to align objects relative to window borders
class Window : public TUI::UI_Object {
    Vector2i get_size() {
        return screen_size;
    }
    private:

    Vector2i screen_size = {0, 0};
    void draw_self(TUI& tui, unsigned input, Vector2i origin) override {
        screen_size = tui.get_screen_size();
    }
};