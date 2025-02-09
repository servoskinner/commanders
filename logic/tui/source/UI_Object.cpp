#include "UI_Object.hpp"

void TUI::UI_Object::draw(TUI& tui, unsigned input, Vector2i origin)
{
    if (visible)
    {
        // Draw self first
        draw_self(tui, input, origin + position);

        // Draw children in depth-wise succession
        for (auto& child : children) {
            if (child->use_absolute_position) {
                child->draw(tui, input, origin);
            }
            else {
                Vector2i child_dimensions = child->get_size();
                Vector2i offset = get_size() - child_dimensions;

                Vector2i anchor_offset = { (offset.x * (int)child->horizontal_anchor)/2, (offset.y * (int)child->vertical_anchor)/2 };

                child->draw(tui, input, origin + position + anchor_offset);
            }
        }
    }
}