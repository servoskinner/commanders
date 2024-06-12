#include "NCurses_commander.hpp"

NCurses_commander::NCurses_commander()
{
    on = true;
    hand = std::vector<Card_info>(0);
    initscr();

    raw();                  // No line buffering for keys
    noecho();               // Do not echo characters in terminal
    curs_set(0);            // Hide cursor   

    start_color();     

    init_pair(CPAIR_INVERTED,       COLOR_BLACK,        COLOR_WHITE);
    init_pair(CPAIR_ACCENT,         COLOR_BRIGHT_BLACK, COLOR_BLACK);
    init_pair(CPAIR_BRIGHT,         COLOR_BRIGHT_WHITE, COLOR_BLACK);
    init_pair(CPAIR_HIGHLIT,        COLOR_BRIGHT_WHITE, COLOR_RED);
    init_pair(CPAIR_HIGHLIT_SUBTLE, COLOR_RED,          COLOR_BLACK);

    init_pair(CPAIR_GRIDCURSOR,     COLOR_BRIGHT_WHITE, COLOR_BLACK);
    init_pair(CPAIR_GRIDSELECTION,  COLOR_BRIGHT_WHITE, COLOR_BRIGHT_RED);
    init_pair(CPAIR_CARD_UNIT,      COLOR_BRIGHT_WHITE, COLOR_CYAN);
    init_pair(CPAIR_CARD_CONTRACT,  COLOR_BRIGHT_WHITE, COLOR_GREEN);
    init_pair(CPAIR_CARD_TACTIC,    COLOR_BRIGHT_WHITE, COLOR_MAGENTA);
    init_pair(CPAIR_CARD_TACTIC_INV,    COLOR_CYAN,     COLOR_BLACK);
    init_pair(CPAIR_CARD_CONTRACT_INV,  COLOR_GREEN,    COLOR_BLACK);
    init_pair(CPAIR_CARD_TACTIC_INV,    COLOR_MAGENTA,  COLOR_BLACK);
    init_pair(CPAIR_UNIT_VALUE,     COLOR_BRIGHT_RED,   COLOR_BLACK);
    init_pair(CPAIR_UNIT_ADVANTAGE, COLOR_BRIGHT_CYAN,  COLOR_BLACK);
    init_pair(CPAIR_CONTRACT_VALUE, COLOR_BRIGHT_GREEN, COLOR_BLACK);
    
    // Stencil Rect used to render grid cells
    grid_cell.set_corners('+');
    grid_cell.set_borders(' ');
    grid_cell.draw_filled = false;
    grid_cell.set_color(CPAIR_ACCENT);

    grid_border.tl_corner = ACS_ULCORNER;
    grid_border.tr_corner = ACS_URCORNER;
    grid_border.bl_corner = ACS_LLCORNER;
    grid_border.br_corner = ACS_LRCORNER;
    grid_border.set_hborders(ACS_HLINE);
    grid_border.set_vborders(ACS_VLINE);
    grid_border.draw_filled = false;
    grid_border.set_color(CPAIR_ACCENT);
    // Stencil for drawing cursor and selection
    grid_highlight.set_all(' ');
    grid_highlight.draw_filled = false;

    grid_capture_area.set_vborders('.');
    grid_capture_area.set_hborders(ACS_HLINE);
    grid_capture_area.set_corners(ACS_HLINE);
    grid_capture_area.set_color(CPAIR_NORMAL);
    grid_capture_area.draw_filled = false;
    // stencils for drawing 
    hand_cards_left.tl_corner = ACS_ULCORNER;
    hand_cards_left.tr_corner = ACS_HLINE;
    hand_cards_left.bl_corner = ACS_LLCORNER;
    hand_cards_left.br_corner = ACS_HLINE;
    hand_cards_left.b_border  = ACS_HLINE;
    hand_cards_left.t_border  = ACS_HLINE;
    hand_cards_left.l_border  = ACS_VLINE;

    hand_cards_right.tl_corner = ACS_HLINE;
    hand_cards_right.tr_corner = ACS_URCORNER;
    hand_cards_right.bl_corner = ACS_HLINE;
    hand_cards_right.br_corner = ACS_LRCORNER;
    hand_cards_right.b_border  = ACS_HLINE;
    hand_cards_right.t_border  = ACS_HLINE;
    hand_cards_right.r_border  = ACS_VLINE;

    x_scale = XSCALE;
    y_scale = YSCALE;

    focus_group = FGROUP_FIELD; //temp, change to hand
    focus_x = 0; focus_y = 0;
}

NCurses_commander::~NCurses_commander()
{
    on = false;
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
    // Process input
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
    if (input == ' ') {
        if (selected) {
            selected = false;
        }
        else {
        selection_x = focus_x;
        selection_y = focus_y;
        selected = true;
        }
    }
    if (input == KEY_ESC)
    {
        on = false;
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

    render_grid();
    render_hand();

    refresh();
}

void NCurses_commander::render_grid()
{
    // Get window params
    int grid_width_sym = grid_width*x_scale;
    int grid_height_sym = grid_height*y_scale;

    int grid_origin_x = (x_term_size - grid_width_sym) / 2;
    if (grid_origin_x < 0) {
        grid_origin_x = 0;
    }
    int grid_origin_y = Y_GRID_OFFSET;
    
    // Render cells
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
    // Render grid border
    grid_border.x = grid_origin_x;
    grid_border.y = grid_origin_y;

    grid_border.width = grid_width_sym;
    grid_border.height = grid_height_sym;

    grid_border.draw();
    // Capture area highlight
    // change this dynamically:
    grid_capture_area.x = grid_origin_x + 3*x_scale;
    grid_capture_area.y = grid_origin_y;

    grid_capture_area.width = 2*x_scale;
    grid_capture_area.height = grid_height_sym;

    grid_capture_area.draw();
    // ___
    // Rander cursors
    if(focus_group == FGROUP_FIELD)
    {
        if(focus_x < 0 || focus_y < 0 || focus_x >= grid_width || focus_y >= grid_height) {
            throw std::runtime_error("NCurses_commander: Invalid grid focus coordinates");
        }
        grid_highlight.width = x_scale;
        grid_highlight.height = y_scale;
        // Selection
        if (selected)
        {
            grid_highlight.set_color(CPAIR_GRIDSELECTION);

            grid_highlight.x = grid_origin_x + selection_x*x_scale;
            grid_highlight.y = grid_origin_y + selection_y*y_scale;

            grid_highlight.draw();
        }
        // Cursor
        grid_highlight.set_color(CPAIR_INVERTED);

        grid_highlight.x = grid_origin_x + focus_x*x_scale;
        grid_highlight.y = grid_origin_y + focus_y*y_scale;

        grid_highlight.draw();
    }
}

void NCurses_commander::render_hand()
{
    int grid_width_sym = grid_width*x_scale;
    int grid_height_sym = grid_height*y_scale;

    int hand_center_origin_x = x_term_size / 2 - x_scale;
    int hand_center_origin_y = grid_height_sym + 2 + Y_GRID_OFFSET;

        Rect empty_marker;
        Text_box empty_text = {"HAND IS EMPTY"};
        
        empty_marker.x = hand_center_origin_x;
        empty_marker.y = hand_center_origin_y;
        empty_marker.width = x_scale*2;
        empty_marker.height = y_scale*2-1;
        empty_marker.set_all('.');
        empty_marker.draw_filled = false;
        empty_marker.set_color(CPAIR_ACCENT);

        empty_text.x = (x_term_size - empty_text.text.size()) / 2;
        empty_text.y = hand_center_origin_y + y_scale;
        empty_text.color = CPAIR_ACCENT;

        empty_marker.draw();
        empty_text.draw();
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

inline void NCurses_commander::Rect::set_hborders(unsigned symbol)
{
    t_border = symbol;
    b_border = symbol;
}

inline void NCurses_commander::Rect::set_vborders(unsigned symbol)
{
    l_border = symbol;
    r_border = symbol;
}

inline void NCurses_commander::Rect::set_borders(unsigned symbol)
{
    t_border = symbol;
    b_border = symbol;
    l_border = symbol;
    r_border = symbol;
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
            mvaddch(orig_y, i, t_border);
            mvaddch(orig_y + height, i, b_border);
        }
        // Vertical borders
        for (int i = orig_y+1; i < orig_y+height; i++) {
            mvaddch(i, orig_x, l_border);
            mvaddch(i, orig_x + width, r_border);
        }
        // Corners
        if (width > 0 && height > 0)
        {
        mvaddch(orig_y, orig_x, tl_corner);
        mvaddch(orig_y, orig_x+width, tr_corner);
        mvaddch(orig_y+height, orig_x, bl_corner);
        mvaddch(orig_y+height, orig_x+width, br_corner);
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

NCurses_commander::Unit_sprite::Unit_sprite(Card_info c_info)
{
    rect.set_all(' ');
    rect.draw_filled = false;
    rect.set_color(CPAIR_CARD_UNIT);

    name.x = 1;
    name.y = 1;
    name.width = XSCALE-2;
    name.height = 2;

    value.x = XSCALE-2;
    value.y = YSCALE-1;
    value.color = CPAIR_UNIT_VALUE;

    advantage.x = 1;
    advantage.y = YSCALE-1;
    advantage.color = CPAIR_UNIT_ADVANTAGE;

    indicator.x = 1;
    indicator.y = 3;
    indicator.color = CPAIR_ACCENT;

    set_card(c_info);
}

void NCurses_commander::Unit_sprite::set_card(Card_info c_info)
{
    card_info = c_info;
    name.text = Description_generator::get().get_card_instance(card_info.global_id).name;

    value.text = std::to_string(card_info.value);
    advantage.text = card_info.advantage > 0 ? std::to_string(card_info.advantage) : " ";

    indicator.text = "";
    indicator.text += card_info.can_move ? (card_info.can_attack ? "~" : "X") : " ";
    indicator.text += card_info.is_overwhelmed ? "!" : " ";
}

NCurses_commander::Card_sprite::Card_sprite(Description_generator::Card_descr c_descr)
{
    
}


