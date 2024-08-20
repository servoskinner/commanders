#pragma once

#include "Commander.hpp"
#include "Description_generator.hpp"
#include "Misc_functions.hpp"
#include "TUI.hpp"

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

class NCurses_commander : public Commander
{
    private: 
    

    public:
    inline const bool is_on() { return on;}
    int x_scale, y_scale;

    NCurses_commander();
    ~NCurses_commander();

    Order get_order() override;
    void process_event(Event event) override;
    void process_order_feedback(int code);
    void apply_updates();

    private:
    bool on;
    int x_term_size, y_term_size; //getmaxyx(stdscr, height, width);

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
        TUI::Text_box name, value, advantage, indicator;
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
        TUI::Text_box name, cost, value, ability_text, flavor_text;
    };

    enum focus_areas {
        FGROUP_HAND,
        FGROUP_FIELD,
        FGROUP_GAME_INFO,
        FGROUP_CONTROLS_INFO,
        FGROUP_CHAT
    };

    int focus_group;
    int focus_x, focus_y, focus_hand_id;
    bool selected;
    bool hand_highlit;
    int selection_x, selection_y;

    TUI::Rect grid_cell;
    TUI::Rect grid_border;
    TUI::Rect grid_capture_area;
    TUI::Rect grid_highlight;
    TUI::Rect hand_highlight;

    TUI::Rect hand_cards_left, hand_cards_right;
    TUI::Text_box bottom_line;
    TUI::Text_box hand_tooltip_r, hand_tooltip_l;
    TUI::Text_box status_message;

    Order pending_order;
    bool order_is_ready;

    void render_UI();
    void render_hand();
    void render_grid();

    void change_focus_area(int focusarea);
    void change_grid_focus(int x, int y);

    inline unsigned get_input() { TUI::get().get_input();};
};