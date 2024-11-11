#include "TUI.hpp"
#include "Misc_functions.hpp"

#include <iostream>

TUI& TUI::get()
{
    static TUI instance;
    return instance;
}

TUI::TUI() 
{
    initscr();

    raw();                  // No line buffering for keys
    noecho();               // Do not echo characters in terminal
    curs_set(0);            // Hide cursor   

    start_color(); 
    // init color pairs
    for(short fore = 0; fore < 16; fore++) {
        for(short back = 0; back < 16; back++) {
            if (fore != COLOR_WHITE || back != COLOR_BLACK) {
                set_color_pair(get_color_code(fore, back), fore, back);
            }
        }
    }
}

TUI::~TUI() 
{
    endwin();
}

unsigned TUI::get_input()
{
    timeout(0);
    unsigned ch = getch();
    if (ch == ERR) {
        return 0;
    }
    return ch;
}

void TUI::UI_Object::draw(unsigned input, int orig_y, int orig_x)
{
    if (visible)
    {
        for (UIobj_ref& child : children) {
            if (child.get().use_absolute_position) {
                child.get().draw(input, orig_y, orig_x);
            }
            else {
                child.get().draw(input, y + orig_y, x + orig_x);
            }
        }
        draw_self(input, orig_y + y, orig_x + x);
    }
}


void TUI::Rect::draw_self(unsigned input, int orig_y, int orig_x)
{
        // Border ______________
        // Horizontal borders
        attron(COLOR_PAIR(get_color_code(t_border.foreground, t_border.background)));
        for (int i = orig_x+1; i < orig_x+width-1; i++) {
            mvaddch(orig_y, i, t_border.symbol);
        }
        attroff(COLOR_PAIR(get_color_code(t_border.foreground, t_border.background)));
        attron(COLOR_PAIR(get_color_code(b_border.foreground, b_border.background)));
        for (int i = orig_x+1; i < orig_x+width-1; i++) {
            mvaddch(orig_y+height-1, i, b_border.symbol);
        }
        attroff(get_color_code(b_border.foreground, b_border.background));

        // Vertical borders
        attron(COLOR_PAIR(get_color_code(l_border.foreground, l_border.background)));
        for (int i = orig_y+1; i < orig_y+height-1; i++) {
            mvaddch(i, orig_x, l_border.symbol);
        }
        attroff(COLOR_PAIR(get_color_code(l_border.foreground, l_border.background)));
        attron(COLOR_PAIR(get_color_code(r_border.foreground, r_border.background)));
        for (int i = orig_y+1; i < orig_y+height-1; i++) {
            mvaddch(i, orig_x+width-1, r_border.symbol);
        }
        attroff(COLOR_PAIR(get_color_code(r_border.foreground, r_border.background)));

        // Corners
        if (width > 0 && height > 0)
        {
        tui.draw_glyph(orig_y, orig_x, tl_corner);
        tui.draw_glyph(orig_y, orig_x+width-1, tr_corner);
        tui.draw_glyph(orig_y+height-1, orig_x, bl_corner);
        tui.draw_glyph(orig_y+height-1, orig_x+width-1, br_corner);
        }
        // Fill ________________
        if(draw_filled)
        {
            attron(COLOR_PAIR(get_color_code(fill.foreground, fill.background)));
            for (int i = orig_y+1; i < orig_y+height-1; i++) {
                for (int j = orig_x+1; j < orig_x+width-1; j++) {
                    mvaddch(i, j, fill.symbol);
                }
            }
            attroff(COLOR_PAIR(get_color_code(fill.foreground, fill.background)));
        }
}

void TUI::Text::draw_self(unsigned input, int orig_y, int orig_x)
{
    attron(COLOR_PAIR(get_color_code(foreground, background)));
    if (width > 0)
    {
        std::vector<std::string> lines = wrap_text(text, width);
        int n_lines = (height > 0 && lines.size() > height) ? height : lines.size();

        for (int i = 0; i < n_lines; i++) {
            mvprintw(orig_y+i, orig_x, lines[i].c_str());
        }
    }
    else {
        mvprintw(orig_y, orig_x, text.c_str());
    }
    attroff(COLOR_PAIR(get_color_code(foreground, background)));
}

void TUI::Scrollable_text::draw_self(unsigned input, int orig_y, int orig_x)
{
    attron(COLOR_PAIR(get_color_code(foreground, background)));
    if (width > 0)
    {
        std::vector<std::string> lines = wrap_text(text, width);
        if (height <= 0) {
            int n_lines = (lines.size() > height) ? height : lines.size();

            for (int i = 0; i < n_lines; i++) {
                mvprintw(orig_y+i, orig_x, lines[i].c_str());
            }
        }
        else {
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
            int hidden_lines = std::max(0, (int)lines.size() - height);
            scroll_pos = std::min(scroll_pos, hidden_lines);

            int n_lines = std::min((int)lines.size() - hidden_lines, height);

            for (int i = 0; i < n_lines; i++) {
                mvprintw(orig_y+i, orig_x, lines[i+scroll_pos].c_str());
            }
        }
    }
    else {
        mvprintw(orig_y, orig_x, text.c_str());
    }
    attroff(COLOR_PAIR(get_color_code(foreground, background)));

}

TUI::Sprite::Sprite(int height, int width, int y, int x) : UI_Object(y, x)
{
    if (width < 1 || height < 1) {
        throw std::invalid_argument("TUI::Sprite::Sprite(): width and height must be greater than zero");
    }

    sprite = std::vector<std::vector<Glyph>>(height, std::vector<Glyph>(width, {' ', 0}));
}

void TUI::Sprite::draw_self(unsigned input, int orig_y, int orig_x)
{
    TUI& tui = TUI::get();
    for (int i = 0; i < sprite.size(); i++){
        for(int j = 0; j < sprite[i].size(); j++) {
            tui.draw_glyph(orig_y + i, orig_x + j, sprite[i][j]);
        }
    }
}

void TUI::Sprite::set_all(Glyph glyph)
{
    for (int i = 0; i < sprite.size(); i++){
        for(int j = 0; j < sprite[i].size(); j++) {
            sprite[i][j] = glyph;
        }
    }
}

void TUI::Text_input::draw_self(unsigned input, int orig_y, int orig_x)
{
    int capacity = -1;
    if (width > 0 && height > 0) {
        capacity = width * height;
    }

    if (is_printable(input) && (text.size() < capacity || capacity == -1)) {
        *std::back_inserter(text) = input;
    }
    if (text.size() > 0 && (input == 127)) {
        text.resize(text.size() - 1);
    }

    Text::draw_self(input, orig_y, orig_x);
}