#pragma once

#include "TUI.hpp"
#include "UI_Object.hpp"
#include "Misc_functions.hpp"

/// @brief Basic non-interactive text.
class Text : public TUI::UI_Object
{
    public:
    Text(std::string text = "", Vector2i constraints = {0, 0}, Vector2i position = {0, 0}, 
            TUI::Color foreground = TUI::WHITE, TUI::Color background = TUI::TRANSPARENT)
        : raw_text(text), constraints(constraints), background(background), foreground(foreground), UI_Object(position)
        {
            set_text(text);
        }

    Text& operator= (const Text& other) = default;

    TUI::Color background = TUI::TRANSPARENT;
    TUI::Color foreground = TUI::WHITE;

    bool fill_max_size = false;

    Vector2i get_size() const override;
    void set_size_limit(Vector2i new_size_limit);

    void set_xlimit(int x_lim);
    void set_ylimit(int y_lim);

    inline std::string get_text() const { return raw_text; }
    void set_text(std::string new_text);

    protected:

    Vector2i constraints;
    std::string raw_text;
    std::vector<std::string> text_lines;
    /// @brief regenerate text_lines when either size constraints or text content have been changed
    virtual void refit_text();

    virtual void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) override;
};