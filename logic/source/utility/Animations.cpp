#include "TUI.hpp"
#include "Animations.hpp"

Animated::Animated(float delay, std::function<void(void)> on_tick) : timer()
{
    timer.events.push_back(Timer::Timed_event(on_tick, delay));
}

void Blinker::tick()
{
    for (UI_Object& child : children) {
        child.visible = !child.visible;
    }
}

void Crazy_box::tick()
{
    offset = (offset += 1) % ((rect.width + rect.height - 2)*2);
}

std::pair<int, int> Crazy_box::position_character(int position) 
{
    position = (position + offset) % ((rect.width + rect.height - 2)*2);

    if (position < rect.width + rect.height - 2) {
        if (position < rect.width - 1) {
        // top edge
            return {rect.y, rect.x + position};
        }
        else {
        // right edge
            position -= rect.width - 1;
            return {rect.y + position, rect.x + (rect.width - 1)};
        }
    }
    else {
        if (position < rect.width*2 + rect.height - 3) {
        // bottom edge
            position -= rect.width + rect.height;
            return {rect.y + (rect.height - 1), rect.x + (rect.height - 1) - position};
        }
        else {
        // left 
            position -= rect.width*2 + rect.height - 3;
            return {rect.y + (rect.height - 1) - position, rect.x};
        }
    }
    return {rect.y, rect.x};
}

void Crazy_box::draw_self(unsigned input, int orig_y, int orig_x)
{
    timer.process();
    rect.draw(input, orig_y, orig_x);
    TUI& tui = TUI::get();
    TUI::Glyph glyph = {'\0', text_color};

    for (int i = 0; i < text.size(); i++) {
        if (text[i] == ' ') {
            continue;
        }
        glyph.symbol = text[i];
        std::pair<int, int> pos = position_character(i);
        tui.draw_glyph(pos.first + orig_y, pos.second + orig_x, glyph);
    }
}

void Rolling_text::tick()
{
    n_drawn_chars++;
    if (n_drawn_chars > origin_text.size()) {
        n_drawn_chars = origin_text.size();
    }
}

void Rolling_text::draw_self(unsigned input, int orig_y, int orig_x)
{   
    timer.process();
    text.text = {origin_text.begin(), origin_text.begin() + n_drawn_chars};
    text.draw(input, orig_x, orig_y);
}