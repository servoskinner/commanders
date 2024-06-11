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

    grid_cell.set_corners('+');
    grid_cell.set_borders(' ');
    grid_cell.draw_filled = false;
    grid_cell.set_color(CPAIR_ACCENT);

    grid_border.tl_corner = ACS_ULCORNER;
    grid_border.tr_corner = ACS_URCORNER;
    grid_border.bl_corner = ACS_LLCORNER;
    grid_border.br_corner = ACS_LRCORNER;
    grid_border.h_border  = ACS_HLINE;
    grid_border.v_border  = ACS_VLINE;
    grid_border.draw_filled = false;
    grid_border.set_color(CPAIR_ACCENT);

    grid_highlight.set_all(' ');
    grid_highlight.set_color(CPAIR_INVERTED);
    grid_highlight.draw_filled = false;

    grid_capture_area.set_all('.');
    grid_capture_area.set_color(CPAIR_ACCENT);
    grid_capture_area.draw_filled = false;

    x_scale = 9;
    y_scale = 5;

    focus_group = FGROUP_FIELD; //temp, change to hand
    focus_x = 0; focus_y = 0;
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

void NCurses_commander::apply_updates()
{
    int input = get_input();

    if((input == 'w' || input == 'W') && focus_y != 0) {
        focus_y--;
    }
    if((input == 'a' || input == 'A') && focus_x != 0) {
        focus_x--;
    }
    if((input == 's' || input == 'S') && focus_y != grid_height-1) {
        focus_y++;
    }
    if((input == 'd' || input == 'D') && focus_x != grid_width-1) {
        focus_x++;
    }

    render_UI();
}

unsigned NCurses_commander::get_input()
{
    timeout(0);
    unsigned ch = getch();
    if (ch == ERR) {
        return 0;
    }
    return ch;
}

void NCurses_commander::render_UI()
{
    erase();
    x_term_size = getmaxx(stdscr);
    y_term_size = getmaxy(stdscr);

    if (x_term_size < 0 || y_term_size < 0) {
        throw std::runtime_error("Screen not initialized");
    }

    // render_hand();
    render_grid();

    refresh();
}

void NCurses_commander::render_grid()
{
    int grid_width_sym = grid_width*x_scale;
    int grid_height_sym = grid_height*y_scale;

    int grid_origin_x = (x_term_size - grid_width_sym) / 2;
    if (grid_origin_x < 0) {
        grid_origin_x = 0;
    }
    int grid_origin_y = Y_GRID_OFFSET;

    grid_cell.width  = x_scale;
    grid_cell.height = y_scale;

    for (int x = 0; x < grid_width; x++) {
        for (int y = 0; y < grid_height; y++) 
        {
            grid_cell.x = grid_origin_x + x*x_scale;
            grid_cell.y = grid_origin_y + y*y_scale;
            grid_cell.draw();
        }
    }
    // change this dynamically:
    grid_capture_area.x = grid_origin_x + 3*x_scale;
    grid_capture_area.y = grid_origin_y;

    grid_capture_area.width = 2*x_scale;
    grid_capture_area.height = grid_height_sym;

    grid_capture_area.draw();
    // ___

    grid_border.x = grid_origin_x;
    grid_border.y = grid_origin_y;

    grid_border.width = grid_width_sym;
    grid_border.height = grid_height_sym;

    //grid_border.draw();

    if(focus_group == FGROUP_FIELD)
    {

        if(focus_x < 0 || focus_y < 0 || focus_x >= grid_width || focus_y >= grid_height) {
            throw std::runtime_error("NCurses_commander: Invalid grid focus coordinates");
        }
        grid_highlight.width = x_scale;
        grid_highlight.height = y_scale;

        grid_highlight.x = grid_origin_x + focus_x*x_scale;
        grid_highlight.y = grid_origin_y + focus_y*y_scale;

        grid_highlight.draw();
    }
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

inline void NCurses_commander::Rect::set_all(unsigned symbol)
{
    set_borders(symbol);
    set_corners(symbol);
    fill = symbol;
} 

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

