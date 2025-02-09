#include "Text.hpp"

void Text::draw_self(TUI& tui, unsigned input, Vector2i origin)
{
    int vertical_offset = 0;
    for (const std::string line : text_lines) {
        tui.print(line, origin + Vector2i(0, vertical_offset), foreground, background);
        vertical_offset++;
    }
}

Vector2i Text::get_size() const {
    if (fill_max_size) {
        return constraints;
    }
    else {
        int y_size = text_lines.size();
        int x_size = 0;

        for (const std::string& line : text_lines) {
            x_size = std::max(x_size, (int)line.size());
        }
        return {x_size, y_size};
    }
}

void Text::set_size_limit(Vector2i new_size_limit) {
    constraints = new_size_limit;
    refit_text();
}

void Text::set_xlimit(int x_lim) {
    constraints.x = x_lim;
    refit_text();
}

void Text::set_ylimit(int y_lim) {
    constraints.y = y_lim;
    refit_text();
}

void Text::set_text(std::string new_text) {
    raw_text = new_text;
    refit_text();
}

void Text::refit_text() {
    text_lines = wrap_text(raw_text, constraints.x);
    if (constraints.y > 0 && text_lines.size() > constraints.y) {
        text_lines.resize(constraints.y);
    }
}