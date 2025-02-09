#include "NCurses_commander.hpp"

NCurses_commander::NCurses_commander()
{
    on = true;  
    TUI& tui = NCursesTUI::get();
    
    // Stencil Rect used to render grid cells
    grid_cell.set_corners({'+'});
    grid_cell.set_borders({' '});
    grid_cell.draw_filled = false;
    grid_cell.set_border_color(TUI::BRIGHTBLACK);

    grid_border.draw_filled = false;
    grid_border.set_border_color(TUI::BRIGHTBLACK);
    // Stencil for drawing cursor and selection
    grid_highlight.set_borders({' '});
    grid_highlight.draw_filled = false;

    hand_highlight.set_border_color(TUI::BRIGHTWHITE);
    hand_highlight.draw_filled = false;

    grid_capture_area.set_vborders({'.'});
    grid_capture_area.set_corners({ACS_HLINE});
    grid_capture_area.set_border_color(TUI::BRIGHTWHITE);
    grid_capture_area.draw_filled = false;
    // stencils for drawing 
    hand_cards_left.size.x = HAND_INACTIVE_CARD_WIDTH;
    hand_cards_left.tr_corner = {ACS_HLINE};
    hand_cards_left.br_corner = {ACS_HLINE};
    hand_cards_left.r_border  = {' '};

    hand_cards_right.size.x = HAND_INACTIVE_CARD_WIDTH;
    hand_cards_right.tl_corner = {ACS_HLINE};
    hand_cards_right.bl_corner = {ACS_HLINE};
    hand_cards_right.l_border  = {' '};

    turn_indicator.position.x = 1;

    bottom_line.position.x = 0;
    bottom_line.set_text(" [~] open menu  |  [ENTER] end turn  |  [Z] examine card  |  [X] player intel  |  [C] open chat  |  [R] clear selection");
    bottom_line.foreground = TUI::BRIGHTBLACK;

    status_message.foreground = TUI::BRIGHTBLACK;
    status_message.set_size_limit({0, 1});
    status_message.set_text("Standing by.");

    hand_tooltip_l.foreground = TUI::BRIGHTBLACK;
    hand_tooltip_l.set_text("< [Q]");
    hand_tooltip_r.foreground = TUI::BRIGHTBLACK;
    hand_tooltip_r.set_text("[E] >");

    urgent_message_border.draw_filled = true;
    urgent_message_border.size.x = 30;
    urgent_message_border.size.y = 9;

    urgent_message_text.position.x = 3;
    urgent_message_text.position.y = 3;
    urgent_message_subtext.position.x = 3;
    urgent_message_subtext.position.y = 5;

    chat_border.set_vborders({'.'});
    chat_border.set_border_color(TUI::BRIGHTBLACK);
    chat_border.position.x = CHAT_X;
    chat_border.size.x = CHAT_WIDTH;
    chat_border.size.y = CHAT_HEIGHT;

    chat_heading.set_text("TACCOM");
    chat_heading.foreground = TUI::BRIGHTBLACK;
    chat_heading.position.x = 1;

    funds_indicator.foreground = TUI::YELLOW;

    points_indicator.size.x = 1;
    points_indicator.position.y = 0;

    contract_name.set_xlimit(24);
    contract_name.set_ylimit(1);
    contract_name.foreground = TUI::BRIGHTWHITE;
    contract_name.children = {contract_duration};

    contract_duration.set_xlimit(4);
    contract_duration.set_ylimit(1);
    contract_duration.position.x = 14;
    contract_duration.foreground = TUI::BRIGHTGREEN; // reused

    chat_input_border_active.position.y = CHAT_HEIGHT - 3;
    chat_input_border_active.size.x = CHAT_WIDTH;
    chat_input_border_active.size.y = 3;

    chat_input_border.position.y = CHAT_HEIGHT - 3;
    chat_input_border.size.x = CHAT_WIDTH;
    chat_input_border.size.y = 3;
    chat_input_border.set_border_color(TUI::BRIGHTBLACK);

    chat_input_border_active.tl_corner.foreground = TUI::CYAN;
    chat_input_border_active.t_border.foreground =  TUI::CYAN;
    chat_input_border_active.tr_corner.foreground = TUI::CYAN;
    chat_input_border_active.set_vborders({ACS_VLINE, TUI::BRIGHTBLACK});
    chat_input_border_active.bl_corner.foreground = TUI::BRIGHTWHITE;
    chat_input_border_active.b_border.foreground =  TUI::BRIGHTWHITE;
    chat_input_border_active.br_corner.foreground = TUI::BRIGHTWHITE;

    scale = {XSCALE, YSCALE};

    chat_border.children = {std::move(chat_input_border), std::move(chat_heading)};
    chat_input_border.children = {std::move(chat_input_border_active)};
    urgent_message_border.children = {std::move(urgent_message_text), std::move(urgent_message_subtext)};

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
        status_message.set_text("Invalid order arguments!");
        break;
    case Commander::Event::ORD_INVTYPE:
        status_message.set_text("Invalid order type!");
        break;
    case Commander::Event::ORD_EXHAUSTED:
        status_message.set_text("Invalid order: action exhausted!");
        break;
    case Commander::Event::ORD_NOFUNDS:
        status_message.set_text("Invalid order: insufficient funds!");
        break;
    case Commander::Event::ORD_NOSELECT:
        status_message.set_text("Invalid order: no subject specified!");
        break;
    case Commander::Event::ORD_NOTARGET:
        status_message.set_text("Invalid order: no target specified!");
        break;
    case Commander::Event::ORD_PERMISSION:
        status_message.set_text("Invalid order: you don't have permission!");
        break;
    }
}

void NCurses_commander::draw(unsigned input)
{
    // Reset focuses here
    if (input != 0) {
        if (focus_message.has_control() && focus_end_turn.has_control() && input != KEY_SUBMIT ||
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

    case KEY_SUBMIT:
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
    TUI& tui = NCursesTUI::get();

    Vector2i term_size = tui.get_screen_size();

    if (term_size.x < 0 || term_size.y < 0) {
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
}

void NCurses_commander::render_grid()
{
    TUI& tui = NCursesTUI::get();

    Vector2i term_size = tui.get_screen_size();
    // Get window params
    int grid_width_sym = game_params.grid_size.second*(scale.x-1)+1;
    int grid_height_sym = game_params.grid_size.first*(scale.y-1)+1;

    int grid_origin_x = (term_size.x - grid_width_sym) / 2;
    if (grid_origin_x < 0) {
        grid_origin_x = 0;
    }
    int grid_origin_y = Y_GRID_OFFSET;
    // Render cells
    grid_cell.size.x  = scale.x;
    grid_cell.size.x = scale.y;
    
    for (int x = 0; x < game_params.grid_size.second; x++) {
        for (int y = 0; y < game_params.grid_size.first; y++) 
        {
            grid_cell.position.x = grid_origin_x + x*(scale.x-1);
            grid_cell.position.y = grid_origin_y + y*(scale.y-1);
            grid_cell.draw(tui);
        }
    }
    // Render grid border
    grid_border.position.x = grid_origin_x;
    grid_border.position.y = grid_origin_y;

    grid_border.size.x = grid_width_sym;
    grid_border.size.y = grid_height_sym;

    grid_border.draw(tui);
    // Capture area highlight
    // change this dynamically:
    grid_capture_area.position.x = grid_origin_x + (game_params.grid_size.second / 2 - 1)*(scale.x-1);
    grid_capture_area.position.y = grid_origin_y;

    grid_capture_area.size.x = (2 + game_params.grid_size.second % 2)*(scale.x-1)+1;
    grid_capture_area.size.y = grid_height_sym;

    grid_capture_area.draw(tui);
    // ___
    // Render units on grid
    Unit_sprite unit_stencil;

    std::vector<std::vector<Card_info>> contracts(game_state.players.size(), std::vector<Card_info>());

    for (const Card_info& c_info : game_state.active_cards) {
        if (c_info.type == CTYPE_UNIT)
        {
            unit_stencil.darken_name = !(c_info.owner_id == active_id);
            unit_stencil.set_card(c_info);
            unit_stencil.position.x = grid_origin_x + c_info.y*(scale.x-1);
            unit_stencil.position.y = grid_origin_y + c_info.x*(scale.y-1);

            unit_stencil.draw(tui);
        }
        else {
            contracts[c_info.owner_id].push_back(c_info);
        }
    }

    // Render contracts
    int left_contracts_origin_x = X_CONTRACTS_OFFSET;
    int left_contracts_origin_y = grid_origin_y + 1;

    int right_contracts_origin_x = term_size.x - X_CONTRACTS_OFFSET - 14 - 4 + 1;
    int right_contracts_origin_y = grid_origin_y + 1;

    TUI::Text contracts_label;
    contracts_label.foreground = TUI::BRIGHTBLACK;
    contracts_label.set_text("CONTRACTS:");

    contracts_label.position.x = left_contracts_origin_x;
    contracts_label.position.y = right_contracts_origin_y;
    contracts_label.draw(tui);

    contracts_label.position.x = right_contracts_origin_x;
    contracts_label.draw(tui);

    // left
    for (int i = 0; i < contracts[0].size(); i++) {
        contract_name.position.x = left_contracts_origin_x;
        contract_name.position.y = left_contracts_origin_y + i + 2;

        contract_name.set_text(Description_generator::get_card_instance(contracts[0][i].card_id).name);
        contract_duration.set_text("(" + std::to_string(contracts[0][i].value) + ")");
        contract_name.draw(tui);
    }
    // right
    for (int i = 0; i < contracts[1].size(); i++) {
        contract_name.position.x = right_contracts_origin_x;
        contract_name.position.y = right_contracts_origin_y + i + 2;

        contract_name.set_text(Description_generator::get_card_instance(contracts[1][i].card_id).name);
        contract_duration.set_text("(" + std::to_string(contracts[1][i].value) + ")");
        contract_name.draw(tui);
    }

    // Render player info
    TUI::Text player_stats_text;
    player_stats_text.foreground = TUI::BRIGHTBLACK;
    player_stats_text.position.y = right_contracts_origin_y + 32;
    player_stats_text.set_xlimit(16);

    // player 1
    player_stats_text.position.x = left_contracts_origin_x;
    Player_info player = game_state.players[0];
    player_stats_text.set_text("FUNDS: " + std::to_string(player.funds) + \
                               "\nHAND:  " + std::to_string(player.hand_size) + \
                               "\nDECK:  " + std::to_string(player.library_size) + \
                               "\nJUNK:  " + std::to_string(player.discard_size));
    player_stats_text.draw(tui);

    // player 2
    player_stats_text.position.x = right_contracts_origin_x;
    player = game_state.players[1];
    player_stats_text.set_text("FUNDS: " + std::to_string(player.funds) + \
                               "\nHAND:  " + std::to_string(player.hand_size) + \
                               "\nDECK:  " + std::to_string(player.library_size) + \
                               "\nJUNK:  " + std::to_string(player.discard_size));
    player_stats_text.draw(tui);

    // Render victory points
    points_indicator.size.x = game_state.players[0].points;
    points_indicator.position.x = grid_origin_x+1;
    points_indicator.set_all({'*', TUI::BRIGHTRED, TUI::TRANSPARENT});
    points_indicator.draw(tui);

    points_indicator.size.x = game_state.players[1].points;
    points_indicator.position.x = grid_origin_x + grid_width_sym - points_indicator.size.x - 1;
    points_indicator.set_all({'*', TUI::BLUE, TUI::TRANSPARENT});
    points_indicator.draw(tui);

    // Render cursors
    if(focus_field.has_control())
    {
        if(cursor_x < 0 || cursor_y < 0 || cursor_x >= game_params.grid_size.second || cursor_y >= game_params.grid_size.first) {
            throw std::runtime_error("NCurses_commander: Invalid grid focus coordinates");
        }
        grid_highlight.size.x = scale.x;
        grid_highlight.size.y = scale.y;
        // Selection
        if (selected)
        {
            grid_highlight.set_border_color(TUI::BLACK, TUI::BRIGHTRED);

            grid_highlight.position.x = grid_origin_x + selection_x*(scale.x-1);
            grid_highlight.position.y = grid_origin_y + selection_y*(scale.y-1);

            grid_highlight.draw(tui);
        }
        // Cursor
        if (selected && cursor_x == selection_x && cursor_y == selection_y) {
            grid_highlight.set_border_color(TUI::BLACK, TUI::BRIGHTWHITE); // Overlaid cursor
        }
        else {
            grid_highlight.set_border_color(TUI::BLACK, TUI::BRIGHTBLUE); // Normal cursor
        }
        
        grid_highlight.position.x = grid_origin_x + cursor_x*(scale.x-1);
        grid_highlight.position.y = grid_origin_y + cursor_y*(scale.y-1);

        grid_highlight.draw(tui);
    }

    bottom_line.position.y = term_size.y-1;
    bottom_line.draw(tui);

    status_message.position.y = 1;
    status_message.position.x = grid_origin_x;
    status_message.set_xlimit(grid_width_sym);
    status_message.draw(tui);
}

void NCurses_commander::render_hand()
{
    TUI& tui = NCursesTUI::get();

    Vector2i term_size = tui.get_screen_size();

    int grid_width_sym = game_params.grid_size.second*(scale.x-1)+1;
    int grid_height_sym = game_params.grid_size.first*(scale.y-1)+1;

    int hand_center_origin_x = term_size.x / 2 - scale.x;
    int hand_center_origin_y = grid_height_sym + Y_HAND_OFFSET + Y_GRID_OFFSET;

    funds_indicator.position.x = hand_center_origin_x;
    funds_indicator.position.y = hand_center_origin_y - 1;
    funds_indicator.set_text("FUNDS: #" + std::to_string(game_state.players[active_id].funds));

    funds_indicator.draw(tui);

    if (game_state.hands[active_id].size() == 0) {
        TUI::Rect empty_marker;
        TUI::Text empty_text = {"HAND IS EMPTY"};
        
        empty_marker.position.x = hand_center_origin_x;
        empty_marker.position.y = hand_center_origin_y;
        empty_marker.size.x = scale.x*2;
        empty_marker.size.y = scale.y*2;
        empty_marker.set_all({'.'});
        empty_marker.draw_filled = false;
        empty_marker.set_border_color(TUI::BRIGHTBLACK);

        empty_text.position.x = (term_size.x - empty_text.get_text().size()) / 2;
        empty_text.position.y = hand_center_origin_y + scale.y;
        empty_text.foreground = TUI::BRIGHTBLACK;

        empty_marker.draw(tui);
        empty_text.draw(tui);
    }
    else {
        if (cursor_hand_id >= game_state.hands[active_id].size()) {
            cursor_hand_id = game_state.hands[active_id].size() - 1;
        }
        Hand_card_sprite focused_card(game_state.hands[active_id][cursor_hand_id]);

        focused_card.position.x = hand_center_origin_x;
        focused_card.position.y = hand_center_origin_y;

        focused_card.scale.x = scale.x;
        focused_card.scale.y = scale.y;

        focused_card.draw(tui);

        if (hand_highlit) 
        {
            hand_highlight.position.x = hand_center_origin_x;
            hand_highlight.position.y = hand_center_origin_y;

            hand_highlight.size.x = scale.x*2;
            hand_highlight.size.y = scale.x*2;

            hand_highlight.draw(tui);
        }

        hand_tooltip_l.position.x = hand_center_origin_x - 3;
        hand_tooltip_l.position.y = hand_center_origin_y + scale.y*2;

        hand_tooltip_r.position.x = hand_center_origin_x + scale.x*2 - 2;
        hand_tooltip_r.position.y = hand_center_origin_y + scale.y*2;

        hand_tooltip_l.draw(tui);
        hand_tooltip_r.draw(tui);

        hand_cards_left.position.x = hand_center_origin_x;
        hand_cards_left.position.y = hand_center_origin_y;
        hand_cards_left.size.y = scale.y*2;

        hand_cards_right.position.x = hand_center_origin_x + 2*scale.x;
        hand_cards_right.position.y = hand_center_origin_y;
        hand_cards_right.size.y = scale.y*2;

        TUI::Color type_colors[] = {TUI::MAGENTA, TUI::GREEN, TUI::BLUE};
        // Render cards to the left
        hand_cards_left.position.x -= HAND_INACTIVE_CARD_WIDTH * cursor_hand_id;
        for (int i = 0; i < cursor_hand_id; i++) {   
            hand_cards_left.set_border_color(type_colors[game_state.hands[active_id][i].type]);
            hand_cards_left.draw(tui);
            hand_cards_left.position.x += HAND_INACTIVE_CARD_WIDTH;
        }
        // Render cards to the right
        for (int i = cursor_hand_id+1; i < game_state.hands[active_id].size(); i++) {
            hand_cards_right.set_border_color(type_colors[game_state.hands[active_id][i].type]);
            hand_cards_right.draw(tui);
            hand_cards_right.position.x += HAND_INACTIVE_CARD_WIDTH;
        }
    }
}

void NCurses_commander::render_peripheral(unsigned input)
{
    TUI& tui = NCursesTUI::get();

    Vector2i term_size = tui.get_screen_size();

    int grid_width_sym = game_params.grid_size.first*(scale.x-1)+1;
    int grid_height_sym = game_params.grid_size.second*(scale.y-1)+1;

    int grid_origin_x = (term_size.x - grid_width_sym) / 2;
    if (grid_origin_x < 0) {
        grid_origin_x = 0;
    }
    int grid_origin_y = Y_GRID_OFFSET;

    chat_border.position.y = term_size.y - CHAT_HEIGHT - CHAT_BOTTOM_OFFSET;

    chat_input_border_active.visible = focus_chat.has_control();
    chat_border.draw(tui);

    if (focus_end_turn.has_control()) 
    {
        urgent_message_border.set_border_color(TUI::BRIGHTWHITE, TUI::RED);
        urgent_message_border.fill.foreground = TUI::BRIGHTWHITE;
        urgent_message_border.fill.background = TUI::RED;

        urgent_message_text.set_text("PRESS ENTER TO PASS TURN");
        urgent_message_text.foreground = TUI::BRIGHTWHITE;
        urgent_message_text.background = TUI::RED;

        urgent_message_subtext.set_text("any other key to cancel");
        urgent_message_subtext.foreground = TUI::BRIGHTCYAN;
        urgent_message_subtext.background = TUI::RED;
    }
    if (focus_game_result.has_control())
    {
        urgent_message_border.set_border_color(TUI::BRIGHTBLUE, TUI::GREEN);
        urgent_message_border.fill.foreground = TUI::BRIGHTBLUE;
        urgent_message_border.fill.background = TUI::GREEN;
        urgent_message_text.set_text("      PLAYER " + std::to_string(winning_player) + " WINS");
        urgent_message_text.foreground = TUI::BRIGHTBLUE;
        urgent_message_text.background = TUI::GREEN;
        urgent_message_subtext.set_text("   press space to exit");
        urgent_message_subtext.foreground = TUI::BRIGHTBLUE;
        urgent_message_subtext.background = TUI::GREEN;
    }
    if (focus_message.has_control())
    {
        urgent_message_border.position.y = Y_GRID_OFFSET + (grid_height_sym - urgent_message_border.size.y) / 2;
        urgent_message_border.position.x = (term_size.x - urgent_message_border.size.x) / 2;
        urgent_message_border.draw(tui);
    }
    if (focus_examine.has_control() && grid[cursor_y][cursor_x].has_value()) {
        Hand_card_sprite examined;
        Card_info info = grid[cursor_y][cursor_x].value();
        examined.set_card(info);

        // position the card, centering it above sprite
        examined.position.x = grid_origin_x + info.y*(scale.x-1) - scale.x*3/2;
        examined.position.y = grid_origin_y + info.x*(scale.y-1) - scale.x/2;

        if(cursor_x < game_params.grid_size.second/2) {
            if (cursor_y < game_params.grid_size.first/2) {
                examined.position.x += scale.x;
                examined.position.y += scale.y;
            }
            else {
                examined.position.x += scale.x;
                examined.position.y -= scale.y;
            }
        }
        else {
            if (cursor_y < game_params.grid_size.first/2) {
                examined.position.x -= scale.x;
                examined.position.y += scale.y;
            }
            else {
                examined.position.x -= scale.x;
                examined.position.y -= scale.y;
            }
        }

        examined.draw(tui);
    }

    turn_indicator.set_text("Turn " + std::to_string(game_state.turn_absolute) + ": Player " + std::to_string(game_state.turn));
    turn_indicator.position.y = term_size.y - 2;
    turn_indicator.draw(tui);
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
    scale.x = XSCALE;
    scale.y = YSCALE;

    rect.set_hborders({ACS_HLINE});
    rect.set_vborders({ACS_VLINE});

    rect.tl_corner = {ACS_ULCORNER};
    rect.tr_corner = {ACS_URCORNER};
    rect.bl_corner = {ACS_LLCORNER};
    rect.br_corner = {ACS_LRCORNER};

    rect.draw_filled = true;
    rect.fill = {' ', TUI::TRANSPARENT, TUI::BLACK};

    rect.set_border_color(TUI::WHITE);

    name.position.x = 1;
    name.position.y = 1;
    name.set_ylimit(2);

    name.foreground = TUI::WHITE;
    value.foreground = TUI::BRIGHTMAGENTA;

    advantage.position.x = 1;
    advantage.foreground = TUI::BRIGHTCYAN;

    indicator.position.x = 1;
    indicator.position.y = 3;
    indicator.foreground = TUI::BRIGHTBLACK;

    set_card(c_info);
}

void NCurses_commander::Unit_sprite::set_card(Card_info c_info)
{
    card_info = c_info;
    name.set_text(Description_generator::get_card_instance(card_info.card_id).name);

    value.set_text(std::to_string(card_info.value));
    advantage.set_text(card_info.advantage != 0 ? std::to_string(card_info.advantage) : "");

    std::string indicator_text = "";
    indicator_text += card_info.can_move ? " " : (card_info.can_attack ? "~" : "X");
    indicator_text += card_info.is_overwhelmed ? "!" : " ";
    indicator.set_text(indicator_text);
}

void NCurses_commander::Unit_sprite::draw_self(TUI& tui, unsigned input, Vector2i origin)
{
    rect.size.x = scale.x;
    rect.size.y = scale.y;

    name.set_xlimit(scale.x-2);
    name.foreground = darken_name ? TUI::BRIGHTBLACK : TUI::WHITE;

    value.position.x = scale.x-value.get_text().size()-1;
    value.position.y = scale.y-2;

    advantage.position.y = scale.y-2;

    rect.draw(tui, input, origin);
    name.draw(tui, input, origin);
    value.draw(tui, input, origin);
    advantage.draw(tui, input, origin);
    indicator.draw(tui, input, origin);
}

NCurses_commander::Hand_card_sprite::Hand_card_sprite(Description_generator::Card_descr c_descr)
{
    scale.x = XSCALE;
    scale.y = YSCALE;

    rect.set_hborders({ACS_HLINE});
    rect.set_vborders({ACS_VLINE});

    rect.tl_corner = {ACS_ULCORNER};
    rect.tr_corner = {ACS_URCORNER};
    rect.bl_corner = {ACS_LLCORNER};
    rect.br_corner = {ACS_LRCORNER};

    rect.draw_filled = true;
    rect.fill = {' ', TUI::TRANSPARENT, TUI::BLACK};

    name.position.x = 1;
    name.position.y = 2;
    name.set_ylimit(4);
    cost.position.x = 1;
    cost.position.y = 1;

    ability_text.position.x = 1;
    ability_text.set_ylimit(3);

    flavor_text.position.x = 1;
    flavor_text.set_ylimit(3);

    cost.foreground = TUI::YELLOW;
    flavor_text.foreground = TUI::BRIGHTBLACK;
    set_desc(c_descr);
}

void NCurses_commander::Hand_card_sprite::set_desc(Description_generator::Card_descr c_descr)
{
    name.set_text(c_descr.name);
    ability_text.set_text(c_descr.ability_text);
    flavor_text.set_text(c_descr.flavor_text);
    cost.set_text("#" + std::to_string(c_descr.cost));
    mnemosprite = c_descr.mnemosprite;

    value.set_text(std::to_string(c_descr.value));
    value.foreground = c_descr.type == CTYPE_UNIT ? TUI::BRIGHTMAGENTA : TUI::BRIGHTGREEN;
    
    TUI::Color colors[] = {TUI::MAGENTA, TUI::GREEN, TUI::BLUE};
    rect.set_border_color(colors[c_descr.type]);
    sprite_color = colors[c_descr.type];
}

void NCurses_commander::Hand_card_sprite::set_card(Card_info c_info)
{
    card_info = c_info;
    set_desc(c_info.card_id);
}

void NCurses_commander::Hand_card_sprite::draw_self(TUI& tui, unsigned input, Vector2i origin)
{
    rect.size.x = scale.x*2;
    rect.size.y = scale.x*2;

    name.set_xlimit(scale.x*2-8);
    flavor_text.set_xlimit(scale.x*2-2);
    ability_text.set_xlimit(scale.x*2-2);

    value.position.x = scale.x*2-value.get_text().size()-1;
    value.position.y = scale.y*2-2;

    flavor_text.position.y  = scale.y*2-5;
    ability_text.position.y = scale.y*2-8;

    rect.draw(tui, input, origin);
    name.draw(tui, input, origin);
    cost.draw(tui, input, origin);
    ability_text.draw(tui, input, origin);
    flavor_text.draw(tui, input, origin);
    value.draw(tui, input, origin);

    int sprite_x = scale.x*2-6;
    int sprite_y = 1;
    // mnemosprite
    TUI::Glyph brush = {' ', TUI::TRANSPARENT, sprite_color};

    for (int x=0; x<3; x++) {
        for(int y=0; y<5; y++) {
            if (mnemosprite[x+y*3]) {
                tui.draw_glyph({x+sprite_x+origin.x, y+sprite_y+origin.y}, brush);
                tui.draw_glyph({5-1-x+sprite_x+origin.x, y+sprite_y+origin.y}, brush);
            }
        }
    }
}