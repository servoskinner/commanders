#pragma once

#include "TUI.hpp"
#include "Text.hpp"
#include "UI_Object.hpp"

/// @brief Text that can be scrolled with input or by modifying its scroll_pos.
class Scrollable_text : public Text
{
    public:
    Scrollable_text(std::string text = "", Vector2i size_limit = {0, 0}, Vector2i position = {0, 0},
                    TUI::Color foreground = TUI::WHITE, TUI::Color background = TUI::TRANSPARENT)
        : Text(text, size_limit, position, foreground, background) {}
    Scrollable_text& operator= (const Scrollable_text& other) = default;

    int scroll_pos = 0; /// Scroll offset in lines.
    bool from_bottom = false;

    Vector2i get_size() const override;

    protected:

    void refit_text() override;
    virtual void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) override;
};