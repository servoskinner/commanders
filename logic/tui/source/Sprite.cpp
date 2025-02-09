#include "Sprite.hpp"

Sprite::Sprite(Vector2i size, Vector2i position) : UI_Object(position)
{
    if (size.x < 1 || size.y < 1) {
        throw std::invalid_argument("TUI::Sprite::Sprite(): width and height must be greater than zero");
    }

    sprite = std::vector<std::vector<Glyph>>(size.y, std::vector<Glyph>(size.x, {' ', TRANSPARENT, TRANSPARENT}));
}

Vector2i Sprite::get_size() const {
    if(sprite.size() == 0) {
        return {0, 0};
    }
    else {
        return {(int)sprite[0].size(), (int)sprite.size()};
    }
};

void Sprite::draw_self(TUI& tui, unsigned input, Vector2i origin)
{
    for (int y = 0; y < sprite.size(); y++){
        for(int x = 0; x < sprite[y].size(); x++) {
            tui.draw_glyph(origin + Vector2i(x, y), sprite[y][x]);
        }
    }
}

void Sprite::set_all(TUI::Glyph glyph)
{
    for (int i = 0; i < sprite.size(); i++){
        for(int j = 0; j < sprite[i].size(); j++) {
            sprite[i][j] = glyph;
        }
    }
}