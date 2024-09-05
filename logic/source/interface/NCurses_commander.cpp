#include "NCurses_commander.hpp"

NCurses_commander::NCurses_commander()
{
    on = true;  
    TUI& tui = TUI::get();

    tui.set_color_pair(CPAIR_INVERTED,       COLOR_BLACK,        COLOR_WHITE);
    tui.set_color_pair(CPAIR_DARK,           COLOR_BRIGHT_BLACK, COLOR_BLACK);
    tui.set_color_pair(CPAIR_INVERTED_DARK,  COLOR_BLACK,        COLOR_BRIGHT_BLACK);
    tui.set_color_pair(CPAIR_BRIGHT,         COLOR_BRIGHT_WHITE, COLOR_BLACK);
    tui.set_color_pair(CPAIR_INVERTED_BRIGHT,COLOR_BLACK,        COLOR_BRIGHT_WHITE);
    tui.set_color_pair(CPAIR_HIGHLIT,        COLOR_BRIGHT_WHITE, COLOR_RED);
    tui.set_color_pair(CPAIR_HIGHLIT_SUBTLE, COLOR_RED,          COLOR_BLACK);

    tui.set_color_pair(CPAIR_BLUE,          COLOR_BLUE,          COLOR_BLACK);
    tui.set_color_pair(CPAIR_BRIGHT_BLUE,   COLOR_BRIGHT_BLUE,   COLOR_BLACK);
    tui.set_color_pair(CPAIR_CYAN,          COLOR_CYAN,          COLOR_BLACK);
    tui.set_color_pair(CPAIR_BRIGHT_CYAN,   COLOR_BRIGHT_CYAN,   COLOR_BLACK);

    tui.set_color_pair(CPAIR_GRIDCURSOR,     COLOR_BLACK,        COLOR_BRIGHT_BLUE);
    tui.set_color_pair(CPAIR_GRIDSELECTION,  COLOR_BLACK,        COLOR_BRIGHT_RED);
    tui.set_color_pair(CPAIR_GRIDCURSOR_OL,  COLOR_BLACK,        COLOR_BRIGHT_WHITE);

    tui.set_color_pair(CPAIR_CARD_UNIT,      COLOR_BRIGHT_WHITE, COLOR_MAGENTA);
    tui.set_color_pair(CPAIR_CARD_CONTRACT,  COLOR_BRIGHT_WHITE, COLOR_GREEN);
    tui.set_color_pair(CPAIR_CARD_TACTIC,    COLOR_BRIGHT_WHITE, COLOR_BLUE);
    tui.set_color_pair(CPAIR_CARD_UNIT_INV,      COLOR_MAGENTA,      COLOR_BLACK);
    tui.set_color_pair(CPAIR_CARD_CONTRACT_INV,  COLOR_GREEN,        COLOR_BLACK);
    tui.set_color_pair(CPAIR_CARD_TACTIC_INV,    COLOR_BLUE,         COLOR_BLACK);

    tui.set_color_pair(CPAIR_UNIT_VALUE,     COLOR_BRIGHT_MAGENTA,   COLOR_BLACK);
    tui.set_color_pair(CPAIR_UNIT_ADVANTAGE, COLOR_BRIGHT_GREEN,     COLOR_BLACK);
    tui.set_color_pair(CPAIR_CONTRACT_VALUE, COLOR_BRIGHT_CYAN,      COLOR_BLACK);
    tui.set_color_pair(CPAIR_CARD_COST,      COLOR_YELLOW,           COLOR_BLACK);

    tui.set_color_pair(CPAIR_TEAM_1_POINTS,  COLOR_BRIGHT_RED,       COLOR_BLACK);
    tui.set_color_pair(CPAIR_TEAM_2_POINTS,  COLOR_CYAN,             COLOR_BLACK);

    tui.set_color_pair(CPAIR_HIGHLIGHT_GREEN, COLOR_BRIGHT_CYAN, COLOR_GREEN);
    
    // Stencil Rect used to render grid cells
    grid_cell.set_corners({'+'});
    grid_cell.set_borders({' '});
    grid_cell.draw_filled = false;
    grid_cell.set_border_color(CPAIR_DARK);

    grid_border.draw_filled = false;
    grid_border.set_border_color(CPAIR_DARK);
    // Stencil for drawing cursor and selection
    grid_highlight.set_borders({' '});
    grid_highlight.draw_filled = false;

    hand_highlight.set_border_color(CPAIR_BRIGHT);
    hand_highlight.draw_filled = false;

    grid_capture_area.set_vborders({'.'});
    grid_capture_area.set_corners({ACS_HLINE});
    grid_capture_area.set_border_color(CPAIR_NORMAL);
    grid_capture_area.draw_filled = false;
    // stencils for drawing 
    hand_cards_left.width = HAND_INACTIVE_CARD_WIDTH;
    hand_cards_left.tr_corner = {ACS_HLINE};
    hand_cards_left.br_corner = {ACS_HLINE};
    hand_cards_left.r_border  = {' '};

    hand_cards_right.width = HAND_INACTIVE_CARD_WIDTH;
    hand_cards_right.tl_corner = {ACS_HLINE};
    hand_cards_right.bl_corner = {ACS_HLINE};
    hand_cards_right.l_border  = {' '};

    turn_indicator.x = 1;

    bottom_line.x = 0;
    bottom_line.text = " [~] open menu  |  [ENTER] end turn  |  [Z] examine card  |  [X] player intel  |  [C] open chat  |  [R] clear selection";
    bottom_line.color = CPAIR_DARK;

    status_message.color = CPAIR_DARK;
    status_message.height = 1;
    status_message.text = "Standing by.";

    hand_tooltip_l.color = CPAIR_DARK;
    hand_tooltip_l.text = "< [Q]";
    hand_tooltip_r.color = CPAIR_DARK;
    hand_tooltip_r.text = "[E] >";

    urgent_message_border.draw_filled = true;
    urgent_message_border.width = 30;
    urgent_message_border.height = 9;

    urgent_message_border.children = {urgent_message_text, urgent_message_subtext};

    urgent_message_text.x = 3;
    urgent_message_text.y = 3;
    urgent_message_subtext.x = 3;
    urgent_message_subtext.y = 5;

    chat_border.children = {chat_input_border, chat_heading};
    chat_input_border.children = {chat_input_border_active};

    chat_border.set_vborders({'.'});
    chat_border.set_border_color(CPAIR_DARK);
    chat_border.x = CHAT_X;
    chat_border.width = CHAT_WIDTH;
    chat_border.height = CHAT_HEIGHT;

    chat_heading.text = "TACCOM";
    chat_heading.color = CPAIR_DARK;
    chat_heading.x = 1;

    funds_indicator.color = CPAIR_CARD_COST;

    points_indicator.height = 1;
    points_indicator.y = 0;

    contract_name.width = 24;
    contract_name.height = 1;
    contract_name.color = CPAIR_BRIGHT;
    contract_name.children = {contract_duration};

    contract_duration.width = 4;
    contract_duration.height = 1;
    contract_duration.x = 14;
    contract_duration.color = CPAIR_UNIT_ADVANTAGE; // reused

    chat_input_border_active.y = CHAT_HEIGHT - 3;
    chat_input_border_active.width = CHAT_WIDTH;
    chat_input_border_active.height = 3;

    chat_input_border.y = CHAT_HEIGHT - 3;
    chat_input_border.width = CHAT_WIDTH;
    chat_input_border.height = 3;
    chat_input_border.set_border_color(CPAIR_DARK);

    chat_input_border_active.tl_corner.color = CPAIR_CYAN;
    chat_input_border_active.t_border.color =  CPAIR_CYAN;
    chat_input_border_active.tr_corner.color = CPAIR_CYAN;
    chat_input_border_active.set_vborders({ACS_VLINE, CPAIR_DARK});
    chat_input_border_active.bl_corner.color = CPAIR_BRIGHT;
    chat_input_border_active.b_border.color =  CPAIR_BRIGHT;
    chat_input_border_active.br_corner.color = CPAIR_BRIGHT;

    x_scale = XSCALE;
    y_scale = YSCALE;

    focus_field.claim_control();
    cursor_x = 0; cursor_y = 0;
    hand_highlit = false;
}

NCurses_commander::~NCurses_commander()
{
    on = false;
}

std::optional<Commander::Order> NCurses_commander::get_order()
{
    std::optional<Order> ord = pending_order;
    pending_order.reset();
    return ord;
}

void NCurses_commander::process_event(Commander::Event event)
{ 
    if(event.type == Event::EV_GAME_WON_BY && event.data.size() > 0)
    {
        winning_player = event.data[0];
        focus_message.claim_control();
        focus_game_result.claim_control();
    }
}

void NCurses_commander::process_order_feedback(int code)
{
    switch (code)
    {
    case Commander::Event::ORD_INVARGS:
        status_message.text = "Invalid order arguments!";
        break;
    case Commander::Event::ORD_INVTYPE:
        status_message.text = "Invalid order type!";
        break;
    case Commander::Event::ORD_EXHAUSTED:
        status_message.text = "Invalid order: action exhausted!";
        break;
    case Commander::Event::ORD_NOFUNDS:
        status_message.text = "Invalid order: insufficient funds!";
        break;
    case Commander::Event::ORD_NOSELECT:
        status_message.text = "Invalid order: no subject specified!";
        break;
    case Commander::Event::ORD_NOTARGET:
        status_message.text = "Invalid order: no target specified!";
        break;
    case Commander::Event::ORD_PERMISSION:
        status_message.text = "Invalid order: you don't have permission!";
        break;
    }
}

void NCurses_commander::draw(unsigned input)
{
    // Reset focuses here
    if (input != 0) {
        if (focus_message.has_control() && focus_end_turn.has_control() && input != KEY_ENTR ||
            focus_examine.has_control()) 
        {
            focus_field.claim_control();
        }
        // ...
    }

    switch (input)
    { // Grid cursor movement _____________________
    case 'w':
    case 'W':
        if(focus_field.has_control() && cursor_y != 0) {
            cursor_y--;
        }
        break;

    case 'a':
    case 'A': // Move cursor 
        if(focus_field.has_control() && cursor_x != 0) {
            cursor_x--;
        }
        break;

    case 's':
    case 'S': // Move cursor down
        if(focus_field.has_control() && cursor_y != game_params.grid_size.first-1) {
            cursor_y++;
        }
        break;

    case 'd':
    case 'D': // Move cursor to the right
        if (focus_field.has_control() && cursor_x != game_params.grid_size.second-1) {
        cursor_x++;
        }
        break;
    // Hand selection movement _________________________
    case 'q':
    case 'Q': // Move hand cursor to the left 
        if (focus_field.has_control() && cursor_hand_id != 0) {
        cursor_hand_id--;
        }
        break;

    case 'e':
    case 'E': // Move hand cursor to the right
        cursor_hand_id++;
        if (focus_field.has_control() && cursor_hand_id >= game_state.hands[active_id].size()) {
            cursor_hand_id = game_state.hands[active_id].size() - 1;
        }
        break;
    // Selection ______________________________________
    case ' ':
        if (focus_field.has_control()) {
            if (selected) {
                // Move, attack, deploy order
                selected = false;
                if (!pending_order.has_value()) {
                    Order ord = {};
                    // deploy
                    if (selection_x == cursor_x && selection_y == cursor_y) {
                        ord.type = Order::ORD_PLAY_CARD;
                        ord.data = {cursor_hand_id, selection_y, selection_x};
                        pending_order.emplace(ord);
                    }
                    else if (abs(selection_x-cursor_x) < 2 && abs(selection_y-cursor_y) < 2) {
                        ord.data = {selection_y, selection_x, cursor_y, cursor_x};
                        // attack
                        if (grid[cursor_y][cursor_x].has_value()) {
                            ord.type = Order::ORD_ATTACK;
                            pending_order.emplace(ord);
                        }
                        // move
                        else if (abs(selection_x-cursor_x) + abs(selection_y-cursor_y) < 2) {
                            ord.type = Order::ORD_MOVE;
                            pending_order.emplace(ord);
                        }
                    }
                }
            }
            else {
                selection_x = cursor_x;
                selection_y = cursor_y;
                selected = true;
            }
        }
        if (focus_message.has_control() && focus_game_result.has_control()) {
            on = false;
        }   
        break;
    // Open chat _________________________________________
    case 'c':
    case 'C':
        if (focus_field.has_control()) {
            focus_chat.claim_control();
        }
        break;
    // Examine card ______________________________________
    case 'z':
    case 'Z':
        if (focus_field.has_control() && grid[cursor_y][cursor_x].has_value()) {
            focus_examine.claim_control();
        }
        break;

    case KEY_ENTR:
        if (focus_message.has_control() && focus_end_turn.has_control() && !pending_order.has_value()) {
            // pass order
            Order ord = {Order::ORD_PASS, {}};
            pending_order.emplace(ord);
            focus_field.claim_control();
        }
        else if (focus_field.has_control()) {
            focus_message.claim_control();
            focus_end_turn.claim_control();
        }
        if (focus_chat.has_control()) {
            focus_field.claim_control();
        }
        break;

    case '`':
    case '~':
        on = false;
        break;
    }

    render_UI(input);
}

void NCurses_commander::render_UI(unsigned input)
{
    erase();
    TUI& tui = TUI::get();

    int x_term_size = tui.get_size().second;
    int y_term_size = tui.get_size().first;

    if (x_term_size < 0 || y_term_size < 0) {
        throw std::runtime_error("Screen not initialized");
    }

    render_grid();
    render_hand();
    render_peripheral(input);

    // TUI::Text debug;
    // debug.y = 0;
    // debug.text = "X: " + std::to_string(cursor_x);
    // debug.draw();
    // debug.y = 1;
    // debug.text = "Y: " + std::to_string(cursor_y);
    // debug.draw();
    // debug.y = 2;
    // debug.text = "Z: " + std::to_string(cursor_hand_id);
    // debug.draw();
    // if (pending_order.has_value()) {
    //     std::string order = std::to_string(pending_order.value().type) + " ";
    //     for (int arg : pending_order.value().data) {
    //         order += std::to_string(arg) + " ";
    //     }
    //     debug.text = order;
    //     debug.y = 3;
    //     debug.draw();
    // }
    // debug.y = 4;
    // debug.text = grid[cursor_y][cursor_x].has_value() ? "unit" : "none";
    // debug.draw();

    refresh();

}

void NCurses_commander::render_grid()
{
    TUI& tui = TUI::get();

    int x_term_size = tui.get_size().second;
    int y_term_size = tui.get_size().first;
    // Get window params
    int grid_width_sym = game_params.grid_size.second*(x_scale-1)+1;
    int grid_height_sym = game_params.grid_size.first*(y_scale-1)+1;

    int grid_origin_x = (x_term_size - grid_width_sym) / 2;
    if (grid_origin_x < 0) {
        grid_origin_x = 0;
    }
    int grid_origin_y = Y_GRID_OFFSET;
    // Render cells
    grid_cell.width  = x_scale;
    grid_cell.height = y_scale;
    
    for (int x = 0; x < game_params.grid_size.second; x++) {
        for (int y = 0; y < game_params.grid_size.first; y++) 
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
    grid_capture_area.x = grid_origin_x + (game_params.grid_size.second / 2 - 1)*(x_scale-1);
    grid_capture_area.y = grid_origin_y;

    grid_capture_area.width = (2 + game_params.grid_size.second % 2)*(x_scale-1)+1;
    grid_capture_area.height = grid_height_sym;

    grid_capture_area.draw();
    // ___
    // Render units on grid
    Unit_sprite unit_stencil;

    std::vector<std::vector<Card_info>> contracts(game_state.players.size(), std::vector<Card_info>());

    for (const Card_info& c_info : game_state.active_cards) {
        if (c_info.type == CTYPE_UNIT)
        {
            unit_stencil.darken_name = !(c_info.owner_id == active_id);
            unit_stencil.set_card(c_info);
            unit_stencil.x = grid_origin_x + c_info.y*(x_scale-1);
            unit_stencil.y = grid_origin_y + c_info.x*(y_scale-1);

            unit_stencil.draw();
        }
        else {
            contracts[c_info.owner_id].push_back(c_info);
        }
    }

    // Render contracts
    int left_contracts_origin_x = X_CONTRACTS_OFFSET;
    int left_contracts_origin_y = grid_origin_y + 1;

    int right_contracts_origin_x = x_term_size - X_CONTRACTS_OFFSET - 14 - 4 + 1;
    int right_contracts_origin_y = grid_origin_y + 1;

    TUI::Text contracts_label;
    contracts_label.color = CPAIR_DARK;
    contracts_label.text = "CONTRACTS:";

    contracts_label.x = left_contracts_origin_x;
    contracts_label.y = right_contracts_origin_y;
    contracts_label.draw();

    contracts_label.x = right_contracts_origin_x;
    contracts_label.draw();

    // left
    for (int i = 0; i < contracts[0].size(); i++) {
        contract_name.x = left_contracts_origin_x;
        contract_name.y = left_contracts_origin_y + i + 2;

        contract_name.text = Description_generator::get_card_instance(contracts[0][i].card_id).name;
        contract_duration.text = "(" + std::to_string(contracts[0][i].value) + ")";
        contract_name.draw();
    }
    // right
    for (int i = 0; i < contracts[1].size(); i++) {
        contract_name.x = right_contracts_origin_x;
        contract_name.y = right_contracts_origin_y + i + 2;

        contract_name.text = Description_generator::get_card_instance(contracts[1][i].card_id).name;
        contract_duration.text = "(" + std::to_string(contracts[1][i].value) + ")";
        contract_name.draw();
    }

    // Render player info
    TUI::Text player_stats_text;
    player_stats_text.color = CPAIR_DARK;
    player_stats_text.y = right_contracts_origin_y + 32;
    player_stats_text.width = 16;

    // player 1
    player_stats_text.x = left_contracts_origin_x;
    Player_info player = game_state.players[0];
    player_stats_text.text = "FUNDS: " + std::to_string(player.funds) + \
                             "\nHAND: " + std::to_string(player.hand_size) + \
                             "\nDECK: " + std::to_string(player.library_size) + \
                             "\nJUNK: " + std::to_string(player.discard_size);
    player_stats_text.draw();

    // player 2
    player_stats_text.x = right_contracts_origin_x;
    player = game_state.players[1];
    player_stats_text.text = "FUNDS: " + std::to_string(player.funds) + \
                             "\nHAND: " + std::to_string(player.hand_size) + \
                             "\nDECK: " + std::to_string(player.library_size) + \
                             "\nJUNK: " + std::to_string(player.discard_size);
    player_stats_text.draw();

    // Render victory points
    points_indicator.width = game_state.players[0].points;
    points_indicator.x = grid_origin_x+1;
    points_indicator.set_all({'*', CPAIR_TEAM_1_POINTS});
    points_indicator.draw();

    points_indicator.width = game_state.players[1].points;
    points_indicator.x = grid_origin_x + grid_width_sym - points_indicator.width - 1;
    points_indicator.set_all({'*', CPAIR_TEAM_2_POINTS});
    points_indicator.draw();

    // Render cursors
    if(focus_field.has_control())
    {
        if(cursor_x < 0 || cursor_y < 0 || cursor_x >= game_params.grid_size.second || cursor_y >= game_params.grid_size.first) {
            throw std::runtime_error("NCurses_commander: Invalid grid focus coordinates");
        }
        grid_highlight.width = x_scale;
        grid_highlight.height = y_scale;
        // Selection
        if (selected)
        {
            grid_highlight.set_border_color(CPAIR_GRIDSELECTION);

            grid_highlight.x = grid_origin_x + selection_x*(x_scale-1);
            grid_highlight.y = grid_origin_y + selection_y*(y_scale-1);

            grid_highlight.draw();
        }
        // Cursor
        if (selected && cursor_x == selection_x && cursor_y == selection_y) {
            grid_highlight.set_border_color(CPAIR_GRIDCURSOR_OL); // Overlaid cursor
        }
        else {
            grid_highlight.set_border_color(CPAIR_GRIDCURSOR); // Normal cursor
        }
        
        grid_highlight.x = grid_origin_x + cursor_x*(x_scale-1);
        grid_highlight.y = grid_origin_y + cursor_y*(y_scale-1);

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
    TUI& tui = TUI::get();

    int x_term_size = tui.get_size().second;
    int y_term_size = tui.get_size().first;

    int grid_width_sym = game_params.grid_size.second*(x_scale-1)+1;
    int grid_height_sym = game_params.grid_size.first*(y_scale-1)+1;

    int hand_center_origin_x = x_term_size / 2 - x_scale;
    int hand_center_origin_y = grid_height_sym + Y_HAND_OFFSET + Y_GRID_OFFSET;

    funds_indicator.x = hand_center_origin_x;
    funds_indicator.y = hand_center_origin_y - 1;
    funds_indicator.text = "FUNDS: #" + std::to_string(game_state.players[active_id].funds);

    funds_indicator.draw();

    if (game_state.hands[active_id].size() == 0) {
        TUI::Rect empty_marker;
        TUI::Text empty_text = {"HAND IS EMPTY"};
        
        empty_marker.x = hand_center_origin_x;
        empty_marker.y = hand_center_origin_y;
        empty_marker.width = x_scale*2;
        empty_marker.height = y_scale*2;
        empty_marker.set_all({'.'});
        empty_marker.draw_filled = false;
        empty_marker.set_border_color(CPAIR_DARK);

        empty_text.x = (x_term_size - empty_text.text.size()) / 2;
        empty_text.y = hand_center_origin_y + y_scale;
        empty_text.color = CPAIR_DARK;

        empty_marker.draw();
        empty_text.draw();
    }
    else {
        if (cursor_hand_id >= game_state.hands[active_id].size()) {
            cursor_hand_id = game_state.hands[active_id].size() - 1;
        }
        Card_sprite focused_card(game_state.hands[active_id][cursor_hand_id]);

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

        hand_tooltip_l.x = hand_center_origin_x - 3;
        hand_tooltip_l.y = hand_center_origin_y + y_scale*2;

        hand_tooltip_r.x = hand_center_origin_x + x_scale*2 - 2;
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
        hand_cards_left.x -= HAND_INACTIVE_CARD_WIDTH * cursor_hand_id;
        for (int i = 0; i < cursor_hand_id; i++) {   
            hand_cards_left.set_border_color(type_colors[game_state.hands[active_id][i].type]);
            hand_cards_left.draw();
            hand_cards_left.x += HAND_INACTIVE_CARD_WIDTH;
        }
        // Render cards to the right
        for (int i = cursor_hand_id+1; i < game_state.hands[active_id].size(); i++) {
            hand_cards_right.set_border_color(type_colors[game_state.hands[active_id][i].type]);
            hand_cards_right.draw();
            hand_cards_right.x += HAND_INACTIVE_CARD_WIDTH;
        }
    }
}

void NCurses_commander::render_peripheral(unsigned input)
{
    TUI& tui = TUI::get();

    int x_term_size = tui.get_size().second;
    int y_term_size = tui.get_size().first;

    int grid_width_sym = game_params.grid_size.first*(x_scale-1)+1;
    int grid_height_sym = game_params.grid_size.second*(y_scale-1)+1;

    int grid_origin_x = (x_term_size - grid_width_sym) / 2;
    if (grid_origin_x < 0) {
        grid_origin_x = 0;
    }
    int grid_origin_y = Y_GRID_OFFSET;

    chat_border.y = y_term_size - CHAT_HEIGHT - CHAT_BOTTOM_OFFSET;

    chat_input_border_active.visible = focus_chat.has_control();
    chat_border.draw();

    if (focus_end_turn.has_control()) 
    {
        urgent_message_border.set_border_color(CPAIR_HIGHLIT);
        urgent_message_border.fill.color = CPAIR_HIGHLIT;
        urgent_message_text.text = "PRESS ENTER TO PASS TURN";
        urgent_message_text.color = CPAIR_HIGHLIT;
        urgent_message_subtext.text = "any other key to cancel";
        urgent_message_subtext.color = CPAIR_HIGHLIT;
    }
    if (focus_game_result.has_control())
    {
        urgent_message_border.set_border_color(CPAIR_HIGHLIGHT_GREEN);
        urgent_message_border.fill.color = CPAIR_HIGHLIGHT_GREEN;
        urgent_message_text.text = "      PLAYER " + std::to_string(winning_player) + " WINS";
        urgent_message_text.color = CPAIR_HIGHLIGHT_GREEN;
        urgent_message_subtext.text = "   press space to exit";
        urgent_message_subtext.color = CPAIR_HIGHLIGHT_GREEN;
    }
    if (focus_message.has_control())
    {
        urgent_message_border.y = Y_GRID_OFFSET + (grid_height_sym - urgent_message_border.height) / 2;
        urgent_message_border.x = (x_term_size - urgent_message_border.width) / 2;
        urgent_message_border.draw();
    }
    if (focus_examine.has_control() && grid[cursor_y][cursor_x].has_value()) {
        Card_sprite examined;
        Card_info info = grid[cursor_y][cursor_x].value();
        examined.set_card(info);

        // position the card, centering it above sprite
        examined.x = grid_origin_x + info.y*(x_scale-1) - x_scale*3/2;
        examined.y = grid_origin_y + info.x*(y_scale-1) - y_scale/2;

        if(cursor_x < game_params.grid_size.second/2) {
            if (cursor_y < game_params.grid_size.first/2) {
                examined.x += x_scale;
                examined.y += y_scale;
            }
            else {
                examined.x += x_scale;
                examined.y -= y_scale;
            }
        }
        else {
            if (cursor_y < game_params.grid_size.first/2) {
                examined.x -= x_scale;
                examined.y += y_scale;
            }
            else {
                examined.x -= x_scale;
                examined.y -= y_scale;
            }
        }

        examined.draw();
    }

    turn_indicator.text = "Turn " + std::to_string(game_state.turn_absolute) + ": Player " + std::to_string(game_state.turn);
    turn_indicator.y = y_term_size - 2;
    turn_indicator.draw();
}

void NCurses_commander::update_state(Game_state state)
{
    game_state = state;
    // clear grid
    for (auto& row : grid) {
        for(std::optional<Card_info>& tile : row) {
            tile.reset();
        }
    }
    // place units
    for (Card_info card : state.active_cards) {
        if (card.type == CTYPE_UNIT) {
            grid[card.x][card.y].emplace(card);
        }
    }
}   

void NCurses_commander::set_params(Game_params params)
{
    game_params = params;
    // build grid
    grid = std::vector<std::vector<std::optional<Card_info>>>(params.grid_size.first, \
                       std::vector<std::optional<Card_info>>(params.grid_size.second));
}

NCurses_commander::Unit_sprite::Unit_sprite(Card_info c_info)
{
    x_scale = XSCALE;
    y_scale = YSCALE;

    rect.set_hborders({ACS_HLINE});
    rect.set_vborders({ACS_VLINE});

    rect.tl_corner = {ACS_ULCORNER};
    rect.tr_corner = {ACS_URCORNER};
    rect.bl_corner = {ACS_LLCORNER};
    rect.br_corner = {ACS_LRCORNER};

    rect.draw_filled = true;
    rect.fill = {' ', 0};

    rect.set_border_color(CPAIR_NORMAL);

    name.x = 1;
    name.y = 1;
    name.height = 2;

    name.color = CPAIR_NORMAL;
    value.color = CPAIR_UNIT_VALUE;

    advantage.x = 1;
    advantage.color = CPAIR_UNIT_ADVANTAGE;

    indicator.x = 1;
    indicator.y = 3;
    indicator.color = CPAIR_DARK;

    set_card(c_info);
}

void NCurses_commander::Unit_sprite::set_card(Card_info c_info)
{
    card_info = c_info;
    name.text = Description_generator::get_card_instance(card_info.card_id).name;

    value.text = std::to_string(card_info.value);
    advantage.text = card_info.advantage != 0 ? std::to_string(card_info.advantage) : "";

    indicator.text = "";
    indicator.text += card_info.can_move ? " " : (card_info.can_attack ? "~" : "X");
    indicator.text += card_info.is_overwhelmed ? "!" : " ";
}

void NCurses_commander::Unit_sprite::draw_self(unsigned input, int orig_y, int orig_x)
{
    rect.width = x_scale;
    rect.height = y_scale;

    name.width = x_scale-2;
    name.color = darken_name ? CPAIR_DARK : CPAIR_NORMAL;

    value.x = x_scale-value.text.size()-1;
    value.y = y_scale-2;

    advantage.y = y_scale-2;

    rect.draw(input, orig_y, orig_x);
    name.draw(input, orig_y, orig_x);
    value.draw(input, orig_y, orig_x);
    advantage.draw(input, orig_y, orig_x);
    indicator.draw(input, orig_y, orig_x);
}

NCurses_commander::Card_sprite::Card_sprite(Description_generator::Card_descr c_descr)
{
    x_scale = XSCALE;
    y_scale = YSCALE;

    rect.set_hborders({ACS_HLINE});
    rect.set_vborders({ACS_VLINE});

    rect.tl_corner = {ACS_ULCORNER};
    rect.tr_corner = {ACS_URCORNER};
    rect.bl_corner = {ACS_LLCORNER};
    rect.br_corner = {ACS_LRCORNER};

    rect.draw_filled = true;
    rect.fill = {' ', 0};

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
    flavor_text.color = CPAIR_DARK;
    set_desc(c_descr);
}

void NCurses_commander::Card_sprite::set_desc(Description_generator::Card_descr c_descr)
{
    name.text = c_descr.name;
    ability_text.text = c_descr.ability_text;
    flavor_text.text = c_descr.flavor_text;
    cost.text = "#" + std::to_string(c_descr.cost);
    mnemosprite = c_descr.mnemosprite;

    value.text = std::to_string(c_descr.value);
    value.color = c_descr.type == CTYPE_UNIT ? CPAIR_UNIT_VALUE : CPAIR_CONTRACT_VALUE;
    
    int type_colors[] = {CPAIR_CARD_UNIT_INV, CPAIR_CARD_CONTRACT_INV, CPAIR_CARD_TACTIC_INV};
    rect.set_border_color(type_colors[c_descr.type]);

    int sprite_colors[] = {CPAIR_CARD_UNIT, CPAIR_CARD_CONTRACT, CPAIR_CARD_TACTIC};
    sprite_color = sprite_colors[c_descr.type];
}

void NCurses_commander::Card_sprite::set_card(Card_info c_info)
{
    card_info = c_info;
    set_desc(c_info.card_id);
}

void NCurses_commander::Card_sprite::draw_self(unsigned input, int orig_y, int orig_x)
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

    rect.draw(input, orig_y, orig_x);
    name.draw(input, orig_y, orig_x);
    cost.draw(input, orig_y, orig_x);
    ability_text.draw(input, orig_y, orig_x);
    flavor_text.draw(input, orig_y, orig_x);
    value.draw(input, orig_y, orig_x);

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