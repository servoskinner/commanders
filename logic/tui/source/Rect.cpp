#include "Rect.hpp"

void Rect::draw_self(TUI& tui, unsigned input, Vector2i origin)
{
        // Borders
        // Horizontal borders
        for (int x = origin.x + 1; x < origin.x + size.x - 1; x++) {
            tui.draw_glyph({x, origin.y}, t_border);
        }
        for (int x = origin.x + 1; x < origin.x + size.x - 1; x++) {
            tui.draw_glyph({x, origin.y + size.y - 1}, b_border);
        }

        // Vertical borders
        for (int y = origin.y + 1; y < origin.y + size.y - 1; y++) {
            tui.draw_glyph({origin.x, y}, l_border);
        }
        for (int y = origin.y + 1; y < origin.y + size.y - 1; y++) {
            tui.draw_glyph({origin.x + size.x - 1, y}, r_border);
        }

        // Corners
        if (size.x > 0 && size.y > 0)
        {
        tui.draw_glyph(origin, tl_corner);
        tui.draw_glyph(origin + Vector2i(size.x-1, 0), tr_corner);
        tui.draw_glyph(origin + Vector2i(0, size.y-1), bl_corner);
        tui.draw_glyph(origin + size - Vector2i(1, 1), br_corner);
        }
        // Fill ________________
        if(draw_filled)
        {
            tui.draw_rect(origin + Vector2i(1, 1), origin + size - Vector2i(1, 1), fill);
        }
}
