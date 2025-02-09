#pragma once

#include "TUI.hpp"
#include "UI_Object.hpp"

/// @brief A drawable patch of individually adjustable Glyphs.
class TUI::Sprite : public TUI::UI_Object
{
    public:
    Sprite(Vector2i size, Vector2i position = {0, 0});
    Sprite& operator=(Sprite &other) { sprite = other.sprite; return *this;};
    Sprite(const Sprite& other) = default;

    Vector2i get_size() const override;

    inline Glyph get_glyph(Vector2i local_pos) {
        if (local_pos >= Vector2i(0, 0) && local_pos.y < sprite.size() && local_pos.x < sprite[0].size()) {
            return sprite[local_pos.y][local_pos.x];
        }
        else {
            return {' ', TRANSPARENT, TRANSPARENT};
        }
    };
    inline void set_glyph(Vector2i local_pos, Glyph glyph) { sprite[local_pos.y][local_pos.x] = glyph;};
    void set_all(Glyph glyph);

    private:
    std::vector<std::vector<Glyph>> sprite;
    virtual void draw_self(TUI& tui, unsigned input = 0, Vector2i origin = {0, 0}) override;
};