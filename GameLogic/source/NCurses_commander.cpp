#include "NCurses_commander.hpp"

NCurses_commander::NCurses_commander()
{
    initscr();

    raw();                  // No line buffering for keys
    noecho();               // Do not echo characters in terminal
    curs_set(0);            // Hide cursor   

    start_color();     
    init_pair(CPAIR_INVERTED,       COLOR_BLACK,        COLOR_WHITE);
    init_pair(CPAIR_ACCENT,         COLOR_BRIGHT_BLACK, COLOR_BLACK);
    init_pair(CPAIR_BRIGHT,         COLOR_BRIGHT_WHITE, COLOR_BLACK);
    init_pair(CPAIR_CYAN_HIGHLT,    COLOR_BRIGHT_WHITE, COLOR_CYAN);
    init_pair(CPAIR_RED_HIGHLT,     COLOR_BRIGHT_WHITE, COLOR_RED);
    init_pair(CPAIR_MAGENTA_HIGHLT, COLOR_BRIGHT_WHITE, COLOR_BLACK);
    init_pair(CPAIR_YELLOW,         COLOR_BLACK,        COLOR_YELLOW);
}

NCurses_commander::~NCurses_commander()
{
    endwin();
}

Commander::Order NCurses_commander::get_order()
{
    return Order();
}

void NCurses_commander::process_event(const Commander::Event& event)
{}

int NCurses_commander::get_input()
{
    timeout(0);
    int ch = getch();
    if (ch == ERR) {
        return 0;
    }
    return ch;
}

void NCurses_commander::apply_updates()
{
    erase();
    x_term_size = getmaxx(stdscr);
    y_term_size = getmaxy(stdscr);

    if (x_term_size < 0 || y_term_size < 0) {
        throw std::runtime_error("Screen not initialized");
    }
    
    refresh();
}

void NCurses_commander::UI_Object::draw(int orig_y, int orig_x)
{
    for (UIobj_ref& child : children) {
        if (child.get().visible) {
            if (child.get().use_absolute_position && child.get().y >= 0 && child.get().x >= 0) {
                child.get().draw(child.get().y, child.get().x);
            }
            else if (child.get().y + orig_y >= 0 && child.get().x + orig_x >= 0) {
                child.get().draw(child.get().y + orig_y, child.get().x + orig_x);
            }
        }
    }
    draw_self(orig_y + y, orig_x + x);
}

inline void NCurses_commander::Rect::set_color(int color)
{
    fill_color   = color;
    border_color = color;
}

inline void NCurses_commander::Rect::set_borders(unsigned symbol)
{
    h_border = symbol;
    v_border = symbol;
}

inline void NCurses_commander::Rect::set_corners(unsigned symbol)
{
    tl_corner = symbol;
    tr_corner = symbol;
    bl_corner = symbol;
    br_corner = symbol;
} 

    // class Card_sprite : UI_Object
    // {
    //     public:
    //     Card_info card_info;
    //     int x_scale = 10, y_scale = 5;
    //     int x_grid = -1, y_grid = -1;

    //     bool focused = false;

    //     private:
    //     virtual void draw_self(int y, int x) override;
    //     Rect rect;
    //     Text_box name;
    //     Text_box value;
    //     Text_box advantage;
    // };
    // class Card_sprite_extended : UI_Object
    // {
    //     public:
    //     int x_scale = 18, y_scale = 7;

    //     Description_generator::Card_descr card_descr;
    //     //...
    //     private:
    //     virtual void draw_self(int y, int x) override;
    // };

void NCurses_commander::Rect::draw_self(int orig_y, int orig_x)
{
        // Border ______________
        if (border_color != 0) {
            attron(COLOR_PAIR(border_color));
        }
        // Horizontal borders
        for (int i = orig_x+1; i < orig_x+width; i++) {
            mvaddch(orig_y, i, h_border);
            mvaddch(orig_y + height, i, h_border);
        }
        // Vertical borders
        for (int i = orig_y+1; i < orig_y+height; i++) {
            mvaddch(i, orig_x, v_border);
            mvaddch(i, orig_x + width, v_border);
        }
        // Corners
        mvaddch(orig_y, orig_x, tl_corner);
        mvaddch(orig_y, orig_x+width, tr_corner);
        mvaddch(orig_y+height, orig_x, bl_corner);
        mvaddch(orig_y+height, orig_x+width, br_corner);

        if (border_color != 0) {
            attroff(COLOR_PAIR(border_color));
        }
        // Fill ________________
        if(draw_filled)
        {
            if (fill_color != 0) {
                attron(COLOR_PAIR(fill_color));
            }
            for (int i = orig_y+1; i < orig_y+height; i++)
            {
                for (int j = orig_x+1; j < orig_x+width; j++)
                {
                    mvaddch(i, j, fill);
                }
            }
            if (fill_color != 0) {
                attroff(COLOR_PAIR(fill_color));
            }
        }
}

void NCurses_commander::Text_box::draw_self(int orig_y, int orig_x)
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

