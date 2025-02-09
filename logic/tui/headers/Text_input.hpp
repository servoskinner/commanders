#pragma once

#include "TUI.hpp"
#include "UI_Object.hpp"
#include "Text.hpp"
#include "Misc_functions.hpp"

/// @brief Dynamic text that can be appended via input.
class Text_input : public Text
{
    public:
    Text_input(Vector2i size_limit = {0, 0}, Vector2i position = {0, 0},
                TUI::Color foreground = TUI::WHITE, TUI::Color background = TUI::TRANSPARENT)
        : Text("", size_limit, position, foreground, background) {}

    Text_input& operator= (const Text_input& other) = default;

    protected:
    virtual void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) override;
};