#include "NCurses_commander.hpp"

NCurses_commander::NCurses_commander()
{
    on = true;  

    init_pair(CPAIR_INVERTED,       COLOR_BLACK,        COLOR_WHITE);
    init_pair(CPAIR_ACCENT,         COLOR_BRIGHT_BLACK, COLOR_BLACK);
    init_pair(CPAIR_BRIGHT,         COLOR_BRIGHT_WHITE, COLOR_BLACK);
    init_pair(CPAIR_HIGHLIT,        COLOR_BRIGHT_WHITE, COLOR_RED);
    init_pair(CPAIR_HIGHLIT_SUBTLE, COLOR_RED,          COLOR_BLACK);

    init_pair(CPAIR_GRIDCURSOR,     COLOR_BLACK,        COLOR_BRIGHT_BLUE);
    init_pair(CPAIR_GRIDSELECTION,  COLOR_BLACK,        COLOR_BRIGHT_RED);
    init_pair(CPAIR_GRIDCURSOR_OL,  COLOR_BLACK,        COLOR_BRIGHT_WHITE);

    init_pair(CPAIR_CARD_UNIT,      COLOR_BRIGHT_WHITE, COLOR_RED);
    init_pair(CPAIR_CARD_CONTRACT,  COLOR_BRIGHT_WHITE, COLOR_CYAN);
    init_pair(CPAIR_CARD_TACTIC,    COLOR_BRIGHT_WHITE, COLOR_MAGENTA);
    init_pair(CPAIR_CARD_UNIT_INV,      COLOR_RED,      COLOR_BLACK);
    init_pair(CPAIR_CARD_CONTRACT_INV,  COLOR_CYAN,    COLOR_BLACK);
    init_pair(CPAIR_CARD_TACTIC_INV,    COLOR_MAGENTA,  COLOR_BLACK);

    init_pair(CPAIR_UNIT_VALUE,     COLOR_BRIGHT_RED,   COLOR_BLACK);
    init_pair(CPAIR_UNIT_ADVANTAGE, COLOR_BRIGHT_CYAN,  COLOR_BLACK);
    init_pair(CPAIR_CONTRACT_VALUE, COLOR_BRIGHT_GREEN, COLOR_BLACK);
    init_pair(CPAIR_CARD_COST,      COLOR_YELLOW,       COLOR_BLACK);
    
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
    grid_highlight.tl_corner = ACS_ULCORNER;
    grid_highlight.tr_corner = ACS_URCORNER;
    grid_highlight.bl_corner = ACS_LLCORNER;
    grid_highlight.br_corner = ACS_LRCORNER;
    grid_highlight.draw_filled = false;

    hand_highlight.set_color(CPAIR_BRIGHT);
    hand_highlight.tl_corner = ACS_ULCORNER;
    hand_highlight.tr_corner = ACS_URCORNER;
    hand_highlight.bl_corner = ACS_LLCORNER;
    hand_highlight.br_corner = ACS_LRCORNER;
    hand_highlight.set_hborders(ACS_HLINE);
    hand_highlight.set_vborders(ACS_VLINE);

    hand_highlight.draw_filled = false;

    grid_capture_area.set_vborders('.');
    grid_capture_area.set_hborders(ACS_HLINE);
    grid_capture_area.set_corners(ACS_HLINE);
    grid_capture_area.set_color(CPAIR_NORMAL);
    grid_capture_area.draw_filled = false;
    // stencils for drawing 
    hand_cards_left.width = HAND_INACTIVE_CARD_WIDTH;
    hand_cards_left.tl_corner = ACS_ULCORNER;
    hand_cards_left.tr_corner = ACS_HLINE;
    hand_cards_left.bl_corner = ACS_LLCORNER;
    hand_cards_left.br_corner = ACS_HLINE;
    hand_cards_left.l_border  = ACS_VLINE;
    hand_cards_left.set_hborders(ACS_HLINE);

    hand_cards_right.width = HAND_INACTIVE_CARD_WIDTH;
    hand_cards_right.tl_corner = ACS_HLINE;
    hand_cards_right.tr_corner = ACS_URCORNER;
    hand_cards_right.bl_corner = ACS_HLINE;
    hand_cards_right.br_corner = ACS_LRCORNER;
    hand_cards_right.r_border  = ACS_VLINE;
    hand_cards_right.set_hborders(ACS_HLINE);

    bottom_line.x = 0;
    bottom_line.color = CPAIR_ACCENT;

    status_message.color = CPAIR_ACCENT;
    status_message.height = 1;
    status_message.text = "Standing by.";
    hand_tooltip_l.color = CPAIR_ACCENT;
    hand_tooltip_l.text = "< Q";
    hand_tooltip_r.color = CPAIR_ACCENT;
    hand_tooltip_r.text = "E >";

    x_scale = XSCALE;
    y_scale = YSCALE;

    focus_group = FGROUP_FIELD; //temp, change to hand
    focus_x = 0; focus_y = 0;
}

NCurses_commander::~NCurses_commander()
{
    on = false;
}

Commander::Order NCurses_commander::get_order()
{
    if (order_is_ready) {
        return pending_order;
        pending_order = {};
    }
    else {
        return Order();
    }
}

void NCurses_commander::process_event(Commander::Event event)
{ }

void NCurses_commander::process_order_feedback(int code)
{
    switch (event.type)
    {
    case Commander::Event::EV_ORDER_CONFIRM:
        status_message.text = "Order executed successfully. Standing by.";
        break;
    case Commander::Event::EV_ORDER_INVALID:
        if (event.data.size() == 0) {
            status_message.text = "Invalid order: no reason specified.";
            break;
        }
        switch (event.data[0])
        {
        case Commander::Event::INVORD_INVARGS:
            status_message.text = "Invalid order arguments!";
            break;
        case Commander::Event::INVORD_INVTYPE:
            status_message.text = "Invalid order type!";
            break;
        case Commander::Event::INVORD_EXHAUSTED:
            status_message.text = "Invalid order: action exhausted!";
            break;
        case Commander::Event::INVORD_NOFUNDS:
            status_message.text = "Invalid order: insufficient funds!";
            break;
        case Commander::Event::INVORD_NOSELECT:
            status_message.text = "Invalid order: no subject specified!";
            break;
        case Commander::Event::INVORD_NOTARGET:
            status_message.text = "Invalid order: no target specified!";
            break;
        case Commander::Event::INVORD_PERMISSION:
            status_message.text = "Invalid order: you don't have permission!";
            break;
        }
    default:
        break;
    }
}

void NCurses_commander::apply_updates()
{
    // Process input
    int input = get_input();

    switch (input)
    {
    case 'w':
    case 'W':
        if(focus_y != 0) {
            focus_y--;
        }
        break;
    case 'a':
    case 'A':
        if(focus_x != 0) {
            focus_x--;
        }
        break;
    case 's':
    case 'S':
        if(focus_y != grid_height-1) {
            focus_y++;
        }
        break;
    case 'd':
    case 'D':
        if (focus_x != grid_width-1) {
        focus_x++;
        }
        break;
    case 'q':
    case 'Q':
        if (focus_hand_id != 0) {
        focus_hand_id--;
        }
        break;
    case 'e':
    case 'E':
        focus_hand_id++;
        if (focus_hand_id >= hands[active_id].size()) {
            focus_hand_id = hands[active_id].size() - 1;
        }
        break;
    case ' ':
        if (selected) {
            selected = false;
        }
        else {
        selection_x = focus_x;
        selection_y = focus_y;
        selected = true;
        }
        break;
    case 'c':
    case 'C':
        hand_highlit = !hand_highlit;
        break;
    case KEY_ESC:
        on = false;
        break;
    }

    render_UI();
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
    int grid_width_sym = grid_width*(x_scale-1)+1;
    int grid_height_sym = grid_height*(y_scale-1)+1;

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
            grid_cell.x = grid_origin_x + x*(x_scale-1);
            grid_cell.y = grid_origin_y + y*(y_scale-1);
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
    grid_capture_area.x = grid_origin_x + (grid_width / 2 - 1)*(x_scale-1);
    grid_capture_area.y = grid_origin_y;

    grid_capture_area.width = (2 + grid_width % 2)*(x_scale-1)+1;
    grid_capture_area.height = grid_height_sym;

    grid_capture_area.draw();
    // ___
    // Render units on grid
    Unit_sprite unit_stencil;
    for (const Card_info& c_info : active_cards) {
        if (c_info.type == CTYPE_UNIT)
        {
            unit_stencil.darken_name = !(c_info.owner_id == active_id);
            unit_stencil.set_card(c_info);
            unit_stencil.x = grid_origin_x + c_info.x*(x_scale-1);
            unit_stencil.y = grid_origin_y + c_info.y*(y_scale-1);

            unit_stencil.draw();
        }
    }
    // Render cursors
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

            grid_highlight.x = grid_origin_x + selection_x*(x_scale-1);
            grid_highlight.y = grid_origin_y + selection_y*(y_scale-1);

            grid_highlight.draw();
        }
        // Cursor
        if (selected && focus_x == selection_x && focus_y == selection_y) {
            grid_highlight.set_color(CPAIR_GRIDCURSOR_OL); // Overlaid cursor
        }
        else {
            grid_highlight.set_color(CPAIR_GRIDCURSOR); // Normal cursor
        }
        
        grid_highlight.x = grid_origin_x + focus_x*(x_scale-1);
        grid_highlight.y = grid_origin_y + focus_y*(y_scale-1);

        grid_highlight.draw();
    }

    bottom_line.y = y_term_size-1;
    bottom_line.draw();

    status_message.y = 1;
    status_message.x = grid_origin_x;
    status_message.width = grid_width_sym;
    status_message.draw();
}

void NCurses_commander::render_hand()
{
    int grid_width_sym = grid_width*(x_scale-1)+1;
    int grid_height_sym = grid_height*(y_scale-1)+1;

    int hand_center_origin_x = x_term_size / 2 - x_scale;
    int hand_center_origin_y = grid_height_sym + Y_HAND_OFFSET + Y_GRID_OFFSET;

    if (hands[active_id].size() == 0) {
        TUI::Rect empty_marker;
        TUI::Text_box empty_text = {"HAND IS EMPTY"};
        
        empty_marker.x = hand_center_origin_x;
        empty_marker.y = hand_center_origin_y;
        empty_marker.width = x_scale*2;
        empty_marker.height = y_scale*2;
        empty_marker.set_all('.');
        empty_marker.draw_filled = false;
        empty_marker.set_color(CPAIR_ACCENT);

        empty_text.x = (x_term_size - empty_text.text.size()) / 2;
        empty_text.y = hand_center_origin_y + y_scale;
        empty_text.color = CPAIR_ACCENT;

        empty_marker.draw();
        empty_text.draw();
    }
    else {
        if (focus_hand_id >= hands[active_id].size()) {
            focus_hand_id = hands[active_id].size() - 1;
        }
        Card_sprite focused_card(hands[active_id][focus_hand_id]);

        focused_card.x = hand_center_origin_x;
        focused_card.y = hand_center_origin_y;

        focused_card.x_scale = x_scale;
        focused_card.y_scale = y_scale;

        focused_card.draw();

        if (hand_highlit) 
        {
            hand_highlight.x = hand_center_origin_x;
            hand_highlight.y = hand_center_origin_y;

            hand_highlight.width = x_scale*2;
            hand_highlight.height = y_scale*2;

            hand_highlight.draw();
        }

        hand_tooltip_l.x = hand_center_origin_x - 2;
        hand_tooltip_l.y = hand_center_origin_y + y_scale*2;

        hand_tooltip_r.x = hand_center_origin_x + x_scale*2 - 1;
        hand_tooltip_r.y = hand_center_origin_y + y_scale*2;

        hand_tooltip_l.draw();
        hand_tooltip_r.draw();

        hand_cards_left.x = hand_center_origin_x;
        hand_cards_left.y = hand_center_origin_y;
        hand_cards_left.height = y_scale*2;

        hand_cards_right.x = hand_center_origin_x + 2*x_scale;
        hand_cards_right.y = hand_center_origin_y;
        hand_cards_right.height = y_scale*2;

        int type_colors[] = {CPAIR_CARD_UNIT_INV, CPAIR_CARD_CONTRACT_INV, CPAIR_CARD_TACTIC_INV};
        // Render cards to the left
        hand_cards_left.x -= HAND_INACTIVE_CARD_WIDTH * focus_hand_id;
        for (int i = 0; i < focus_hand_id; i++) {   
            hand_cards_left.set_color(type_colors[hands[active_id][i].type]);
            hand_cards_left.draw();
            hand_cards_left.x += HAND_INACTIVE_CARD_WIDTH;
        }
        // Render cards to the right
        for (int i = focus_hand_id+1; i < hands[active_id].size(); i++) {
            hand_cards_right.set_color(type_colors[hands[active_id][i].type]);
            hand_cards_right.draw();
            hand_cards_right.x += HAND_INACTIVE_CARD_WIDTH;
        }
    }
}

NCurses_commander::Unit_sprite::Unit_sprite(Card_info c_info)
{
    x_scale = XSCALE;
    y_scale = YSCALE;

    rect.set_hborders(ACS_HLINE);
    rect.set_vborders(ACS_VLINE);

    rect.tl_corner = ACS_ULCORNER;
    rect.tr_corner = ACS_URCORNER;
    rect.bl_corner = ACS_LLCORNER;
    rect.br_corner = ACS_LRCORNER;

    rect.draw_filled = false;
    rect.set_color(CPAIR_NORMAL);

    name.x = 1;
    name.y = 1;
    name.height = 2;

    name.color = CPAIR_NORMAL;
    value.color = CPAIR_UNIT_VALUE;

    advantage.x = 1;
    advantage.color = CPAIR_UNIT_ADVANTAGE;

    indicator.x = 1;
    indicator.y = 3;
    indicator.color = CPAIR_ACCENT;

    set_card(c_info);
}

void NCurses_commander::Unit_sprite::set_card(Card_info c_info)
{
    card_info = c_info;
    name.text = Description_generator::get().get_card_instance(card_info.card_id).name;

    value.text = std::to_string(card_info.value);
    advantage.text = card_info.advantage > 0 ? std::to_string(card_info.advantage) : "";

    indicator.text = "";
    indicator.text += card_info.can_move ? (card_info.can_attack ? "~" : "X") : " ";
    indicator.text += card_info.is_overwhelmed ? "!" : " ";
}

void NCurses_commander::Unit_sprite::draw_self(int orig_y, int orig_x)
{
    rect.width = x_scale;
    rect.height = y_scale;

    name.width = x_scale-2;
    name.color = darken_name ? CPAIR_ACCENT : CPAIR_NORMAL;

    value.x = x_scale-value.text.size()-1;
    value.y = y_scale-2;

    advantage.y = y_scale-2;

    rect.draw(orig_y, orig_x);
    name.draw(orig_y, orig_x);
    value.draw(orig_y, orig_x);
    advantage.draw(orig_y, orig_x);
    indicator.draw(orig_y, orig_x);
}

NCurses_commander::Card_sprite::Card_sprite(Description_generator::Card_descr c_descr)
{
    x_scale = XSCALE;
    y_scale = YSCALE;

    rect.set_hborders(ACS_HLINE);
    rect.set_vborders(ACS_VLINE);

    rect.tl_corner = ACS_ULCORNER;
    rect.tr_corner = ACS_URCORNER;
    rect.bl_corner = ACS_LLCORNER;
    rect.br_corner = ACS_LRCORNER;

    name.x = 1;
    name.y = 2;
    name.height = 4;
    cost.x = 1;
    cost.y = 1;

    ability_text.x = 1;
    ability_text.height = 3;

    flavor_text.x = 1;
    flavor_text.height = 3;

    cost.color = CPAIR_CARD_COST;
    flavor_text.color = CPAIR_ACCENT;
    set_desc(c_descr);
}

void NCurses_commander::Card_sprite::set_desc(Description_generator::Card_descr c_descr)
{
    name.text = c_descr.name;
    ability_text.text = c_descr.ability_text;
    flavor_text.text = c_descr.flavor_text;
    cost.text = std::to_string(c_descr.cost);
    mnemosprite = c_descr.mnemosprite;

    value.text = std::to_string(c_descr.value);
    value.color = c_descr.type == CTYPE_UNIT ? CPAIR_UNIT_VALUE : CPAIR_CONTRACT_VALUE;
    
    int type_colors[] = {CPAIR_CARD_UNIT_INV, CPAIR_CARD_CONTRACT_INV, CPAIR_CARD_TACTIC_INV};
    rect.set_color(type_colors[c_descr.type]);

    int sprite_colors[] = {CPAIR_CARD_UNIT, CPAIR_CARD_CONTRACT, CPAIR_CARD_TACTIC};
    sprite_color = sprite_colors[c_descr.type];
}

void NCurses_commander::Card_sprite::set_card(Card_info c_info)
{
    card_info = c_info;
    set_desc(c_info.card_id);
}

void NCurses_commander::Card_sprite::draw_self(int orig_y, int orig_x)
{
    rect.width = x_scale*2;
    rect.height = y_scale*2;

    name.width = x_scale*2-8;
    flavor_text.width = x_scale*2-2;
    ability_text.width = x_scale*2-2;

    value.x = x_scale*2-value.text.size()-1;
    value.y = y_scale*2-2;

    flavor_text.y  = y_scale*2-5;
    ability_text.y = y_scale*2-8;

    rect.draw(orig_y, orig_x);
    name.draw(orig_y, orig_x);
    cost.draw(orig_y, orig_x);
    ability_text.draw(orig_y, orig_x);
    flavor_text.draw(orig_y, orig_x);
    value.draw(orig_y, orig_x);

    int sprite_x = x_scale*2-6;
    int sprite_y = 1;
    // mnemosprite
    
    attron(COLOR_PAIR(sprite_color));
    for (int x=0; x<3; x++) {
        for(int y=0; y<5; y++) {
            if (mnemosprite[x+y*3]) {
                mvaddch(y+sprite_y+orig_y,
                        x+sprite_x+orig_x,
                        ' ');
                mvaddch(y+sprite_y+orig_y,
                        5-1-x+sprite_x+orig_x,
                        ' ');
            }
        }
    }
    attroff(COLOR_PAIR(sprite_color));
}

