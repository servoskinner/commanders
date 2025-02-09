#include "Scrollable_text.hpp"

void Scrollable_text::draw_self(TUI& tui, unsigned input, Vector2i origin)
{
    if (constraints.y > 0 && text_lines.size() > constraints.y)
    {
        switch (input)
        {
        case 'w':
        case 'W':
            if (scroll_pos != 0) {
            scroll_pos--;
            }
            break;
        case 's':
        case 'S':
            scroll_pos++;
            break;
        }
        int hidden_lines = text_lines.size() - constraints.y;
        scroll_pos = std::min(scroll_pos, hidden_lines);

        int n_lines = std::min((int)text_lines.size() - hidden_lines, constraints.y);

        for (int i = 0; i < n_lines; i++) {
            tui.print(text_lines[i+scroll_pos], origin + Vector2i(0, i), foreground, background);
        }
    }
    else {
        Text::draw_self(tui, input, origin);
        scroll_pos = 0;
    }
}

Vector2i Scrollable_text::get_size() const {
    if (fill_max_size) {
        return constraints;
    }
    else {
        int y_size = constraints.y > 0 ? std::min((int)text_lines.size(), constraints.y) : text_lines.size();
        int x_size = 0;

        for (const std::string& line : text_lines) {
            x_size = std::max(x_size, (int)line.size());
        }
        return {x_size, y_size};
    }
}

void Scrollable_text::refit_text() {
    text_lines = wrap_text(raw_text, constraints.x);
} 