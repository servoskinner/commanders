#pragma once

#include "TUI.hpp"

/// @brief Generic TUI Object, multiple can be composed in hierarchical order.
class TUI::UI_Object
{
    public:
    UI_Object(int y = 0, int x = 0) : position(x, y) {}
    UI_Object(Vector2i pos) : position(pos) {}
    UI_Object& operator= (const UI_Object& other) = default;

    Vector2i position; 
    /// @brief If set to true, ignore positioning relative to parent

    bool use_absolute_position = false;
    bool visible = true;
    std::vector<std::shared_ptr<UI_Object>> children = {};

    /// @brief Describes alignment relative to parent.
    enum Anchor {
        START,
        MIDDLE,
        END
    };

    Anchor vertical_anchor = START;
    Anchor horizontal_anchor = START;

    void draw(TUI& tui, unsigned input = 0, Vector2i origin = {});

    virtual Vector2i get_size() const {
        return {0, 0};
    }

    protected:
    virtual void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) {}
};