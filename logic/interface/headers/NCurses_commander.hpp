#pragma once

#include "Commander.hpp"
#include "Description_generator.hpp"
#include "Misc_functions.hpp"
#include "NCursesTUI.hpp"
#include "Sprite.hpp"
#include "Text.hpp"
#include "Rect.hpp"
#include "Focus.hpp"

#include <optional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>

#include "Timer.hpp"

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
    Vector2i scale;

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

    /// @brief Draws a unit card that is placed on board
    class Unit_sprite : public TUI::UI_Object
    {
        public:
        Unit_sprite(Card_info c_info = {});
        void set_card(Card_info c_info);
        const inline Card_info get_card() { return card_info;}
        
        Vector2i scale = {XSCALE, YSCALE};
        bool darken_name;

        private:
        Card_info card_info;
        virtual void draw_self(TUI& tui, unsigned input, Vector2i origin) override;

        Rect rect;
        Text name, value, advantage, indicator;

        public:
        virtual Vector2i get_size() const override {
            return rect.get_size();
        }
    };

    /// @brief Draws the full version of card, as seen in hand or when examined.
    class Hand_card_sprite : public TUI::UI_Object
    {
        public:
        Hand_card_sprite(Description_generator::Card_descr c_descr = {});
        Hand_card_sprite(Card_info c_info) : Hand_card_sprite(Description_generator::get_desc(c_info.card_id)) { card_info = c_info;}
        Hand_card_sprite(int id) : Hand_card_sprite(Description_generator::get_desc(id)) {}

        void set_card(Card_info c_info);
        inline const std::optional<Card_info> get_card() { return card_info;}

        Vector2i scale = {XSCALE*2, YSCALE*2-1};

        //...
        private:
        Description_generator::Card_descr card_descr;
        std::optional<Card_info> card_info;
        std::array<bool, 15> mnemosprite;


        virtual void draw_self(TUI& tui, unsigned input, Vector2i origin) override;

        TUI::Color sprite_color;
        Rect rect;
        Text name, cost, value, ability_text, flavor_text;

        public:
        virtual Vector2i get_size() const override {
            return rect.get_size();
        }
    };

    class Board : public TUI::UI_Object {
        Rect board_frame

        protected:
        void draw_self(TUI& tui, unsigned input, Vector2i origin) override;
    }

    class Hand : public TUI::UI_Object {

        protected:
        void draw_self(TUI& tui, unsigned input, Vector2i origin) override;
    }

    Focus::Focus_space fspace_game, fspace_message;

    Focus focus_field = {fspace_game}, focus_message = {fspace_game}, focus_chat = {fspace_game};
    Focus focus_examine = {fspace_game}, focus_comms = {fspace_game};

    Focus focus_game_result = {fspace_message}, focus_end_turn = {fspace_message};

    int cursor_x, cursor_y, cursor_hand_id;
    bool selected;
    bool hand_highlit;
    int selection_x, selection_y;

    Rect grid_cell;
    Rect grid_border;
    Rect grid_capture_area;
    Rect grid_highlight;
    Rect hand_highlight;

    Text funds_indicator;

    Text contract_name;
    Text contract_duration;

    Rect points_indicator;
    Text turn_indicator;

    Rect hand_cards_left, hand_cards_right;
    Text bottom_line;
    Text hand_tooltip_r, hand_tooltip_l;
    Text status_message;

    Rect urgent_message_border;
    Text urgent_message_text;
    Text urgent_message_subtext;
    int winning_player;

    Rect chat_border;
    Rect chat_input_border;
    Rect chat_input_border_active;
    Text chat_heading;

    std::optional<Order> pending_order;
    std::vector<std::vector<std::optional<Card_info>>> grid;

    void render_UI(unsigned input);
    void render_hand();
    void render_grid();
    void render_peripheral(unsigned input);

    void change_focus_area(int focusarea);
    void change_grid_focus(int x, int y);
};