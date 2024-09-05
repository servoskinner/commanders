#pragma once

#include "Commander.hpp"
#include "Description_generator.hpp"
#include "Misc_functions.hpp"
#include "TUI.hpp"
#include "Focus.hpp"

#include <optional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>

#define XSCALE          10
#define YSCALE          7  
#define HAND_INACTIVE_CARD_WIDTH 2

#define Y_GRID_OFFSET   2
#define Y_HAND_OFFSET   1

#define X_CONTRACTS_OFFSET 4

#define CHAT_X              2
#define CHAT_BOTTOM_OFFSET  3
#define CHAT_WIDTH          30
#define CHAT_HEIGHT         16

#define CPAIR_NORMAL            0
#define CPAIR_INVERTED          1
#define CPAIR_DARK              2
#define CPAIR_INVERTED_DARK     3
#define CPAIR_BRIGHT            4
#define CPAIR_INVERTED_BRIGHT   5
#define CPAIR_HIGHLIT           6
#define CPAIR_HIGHLIT_SUBTLE    7

#define CPAIR_BLUE              8
#define CPAIR_BRIGHT_BLUE       9
#define CPAIR_CYAN              10
#define CPAIR_BRIGHT_CYAN       11

#define CPAIR_GRIDCURSOR        12
#define CPAIR_GRIDSELECTION     13
#define CPAIR_GRIDCURSOR_OL     14
#define CPAIR_CARD_UNIT         15
#define CPAIR_CARD_CONTRACT     16
#define CPAIR_CARD_TACTIC       17   
#define CPAIR_CARD_UNIT_INV     18
#define CPAIR_CARD_CONTRACT_INV 19 
#define CPAIR_CARD_TACTIC_INV   20

#define CPAIR_UNIT_VALUE        21
#define CPAIR_UNIT_ADVANTAGE    22
#define CPAIR_CONTRACT_VALUE    23
#define CPAIR_CARD_COST         24

#define CPAIR_TEAM_1_POINTS     25
#define CPAIR_TEAM_2_POINTS     26

#define CPAIR_HIGHLIGHT_GREEN   27

class NCurses_commander : public Commander
{
    private: 
    

    public:
    inline const bool is_on() { return on;}
    int x_scale, y_scale;

    NCurses_commander();
    ~NCurses_commander();

    std::optional<Order> get_order() override;
    void process_event(Event event) override;
    void process_order_feedback(int code);
    
    void draw(unsigned input);
    void update_state(Game_state state) override;
    void set_params(Game_params params) override;

    private:
    bool on;

    class Unit_sprite : public TUI::UI_Object
    {
        public:
        Unit_sprite(Card_info c_info = {});
        void set_card(Card_info c_info);
        const inline Card_info get_card() { return card_info;}
        
        int x_scale = XSCALE, y_scale = YSCALE;
        bool darken_name;

        private:
        Card_info card_info;
        virtual void draw_self(unsigned input, int y, int x) override;

        TUI::Rect rect;
        TUI::Text name, value, advantage, indicator;
    };
    class Card_sprite : public TUI::UI_Object
    {
        public:
        Card_sprite(Description_generator::Card_descr c_descr = {});
        Card_sprite(Card_info c_info) : Card_sprite(Description_generator::get_card_instance(c_info.card_id)) { card_info = c_info;}
        Card_sprite(int id) : Card_sprite(Description_generator::get_card_instance(id)) {}

        void set_desc(Description_generator::Card_descr c_descr);
        void set_desc(int id) { set_desc(Description_generator::get_card_instance(id));};
        inline const Description_generator::Card_descr get_desc() { return card_descr;}

        void set_card(Card_info c_info);
        inline const void reset_card() { card_info.reset();}
        inline const std::optional<Card_info> get_card() { return card_info;}

        int x_scale = XSCALE*2, y_scale = YSCALE*2-1;

        //...
        private:
        Description_generator::Card_descr card_descr;
        std::optional<Card_info> card_info;
        std::array<bool, 15> mnemosprite;
        int sprite_color;

        virtual void draw_self(unsigned input, int orig_y, int orig_x) override;

        TUI::Rect rect;
        TUI::Text name, cost, value, ability_text, flavor_text;
    };

    Focus::Focus_space fspace_game, fspace_message;

    Focus focus_field = {fspace_game}, focus_message = {fspace_game}, focus_chat = {fspace_game};
    Focus focus_examine = {fspace_game}, focus_comms = {fspace_game};

    Focus focus_game_result = {fspace_message}, focus_end_turn = {fspace_message};

    int cursor_x, cursor_y, cursor_hand_id;
    bool selected;
    bool hand_highlit;
    int selection_x, selection_y;

    TUI::Rect grid_cell;
    TUI::Rect grid_border;
    TUI::Rect grid_capture_area;
    TUI::Rect grid_highlight;
    TUI::Rect hand_highlight;

    TUI::Text funds_indicator;

    TUI::Text contract_name;
    TUI::Text contract_duration;

    TUI::Rect points_indicator;
    TUI::Text turn_indicator;

    TUI::Rect hand_cards_left, hand_cards_right;
    TUI::Text bottom_line;
    TUI::Text hand_tooltip_r, hand_tooltip_l;
    TUI::Text status_message;

    TUI::Rect urgent_message_border;
    TUI::Text urgent_message_text;
    TUI::Text urgent_message_subtext;
    int winning_player;

    TUI::Rect chat_border;
    TUI::Rect chat_input_border;
    TUI::Rect chat_input_border_active;
    TUI::Text chat_heading;

    std::optional<Order> pending_order;
    std::vector<std::vector<std::optional<Card_info>>> grid;

    void render_UI(unsigned input);
    void render_hand();
    void render_grid();
    void render_peripheral(unsigned input);

    void change_focus_area(int focusarea);
    void change_grid_focus(int x, int y);
};