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
    std::cout << "created TUI" << std::endl;
    initscr();

    raw();                  // No line buffering for keys
    noecho();               // Do not echo characters in terminal
    curs_set(0);            // Hide cursor   

    start_color(); 
}

TUI::~TUI() 
{
    std::cout << "destroyed TUI" << std::endl;
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
    for (UIobj_ref& child : children) {
        if (child.get().visible) {
            if (child.get().use_absolute_position) {
                child.get().draw(input, child.get().y, child.get().x);
            }
            else {
                child.get().draw(input, child.get().y + orig_y, child.get().x + orig_x);
            }
        }
    }
    draw_self(input, orig_y + y, orig_x + x);
}


void TUI::Rect::draw_self(unsigned input, int orig_y, int orig_x)
{
        // Border ______________
        if (border_color != 0) {
            attron(COLOR_PAIR(border_color));
        }
        // Horizontal borders
        for (int i = orig_x+1; i < orig_x+width-1; i++) {
            mvaddch(orig_y, i, t_border);
            mvaddch(orig_y+height-1, i, b_border);
        }
        // Vertical borders
        for (int i = orig_y+1; i < orig_y+height-1; i++) {
            mvaddch(i, orig_x, l_border);
            mvaddch(i, orig_x+width-1, r_border);
        }
        // Corners
        if (width > 0 && height > 0)
        {
        mvaddch(orig_y, orig_x, tl_corner);
        mvaddch(orig_y, orig_x+width-1, tr_corner);
        mvaddch(orig_y+height-1, orig_x, bl_corner);
        mvaddch(orig_y+height-1, orig_x+width-1, br_corner);
        }
        if (border_color != 0) {
            attroff(COLOR_PAIR(border_color));
        }
        // Fill ________________
        if(draw_filled)
        {
            if (fill_color != 0) {
                attron(COLOR_PAIR(fill_color));
            }
            for (int i = orig_y+1; i < orig_y+height-1; i++)
            {
                for (int j = orig_x+1; j < orig_x+width-1; j++)
                {
                    mvaddch(i, j, fill);
                }
            }
            if (fill_color != 0) {
                attroff(COLOR_PAIR(fill_color));
            }
        }
}

void TUI::Text_box::draw_self(unsigned input, int orig_y, int orig_x)
{
    if (color != 0) {
        attron(COLOR_PAIR(color));
    }
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
    if (color != 0) {
        attroff(COLOR_PAIR(color));
    }
}

void TUI::Scroll_box::draw_self(unsigned input, int orig_y, int orig_x)
{
    if (color != 0) {
        attron(COLOR_PAIR(color));
    }
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
            if (focus.has_control()) {
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
    if (color != 0) {
        attroff(COLOR_PAIR(color));
    }

}