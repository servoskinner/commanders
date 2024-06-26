#pragma once

#include "Commander.hpp"
#include "Description_generator.hpp"
#include "Misc_functions.hpp"

#include <ncurses.h>

#include <optional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>

#define COLOR_BRIGHT_BLACK		8
#define COLOR_BRIGHT_RED		9
#define COLOR_BRIGHT_GREEN		10
#define COLOR_BRIGHT_YELLOW		11
#define COLOR_BRIGHT_BLUE		12
#define COLOR_BRIGHT_MAGENTA	13
#define COLOR_BRIGHT_CYAN		14
#define COLOR_BRIGHT_WHITE		15

#define CPAIR_NORMAL            0
#define CPAIR_INVERTED          1
#define CPAIR_ACCENT            2
#define CPAIR_BRIGHT            3
#define CPAIR_HIGHLIT           4
#define CPAIR_HIGHLIT_SUBTLE    5

#define CPAIR_GRIDCURSOR        6
#define CPAIR_GRIDSELECTION     7
#define CPAIR_GRIDCURSOR_OL     8
#define CPAIR_CARD_UNIT         9
#define CPAIR_CARD_CONTRACT     10
#define CPAIR_CARD_TACTIC       11   
#define CPAIR_CARD_UNIT_INV     12
#define CPAIR_CARD_CONTRACT_INV 13 
#define CPAIR_CARD_TACTIC_INV   14

#define CPAIR_UNIT_VALUE        15
#define CPAIR_UNIT_ADVANTAGE    16
#define CPAIR_CONTRACT_VALUE    17
#define CPAIR_CARD_COST         18

#define KEY_ESC         27
#define KEY_TAB         9
#define KEY_ENTR        10

#define KEY_UARROW      65
#define KEY_DARROW      66
#define KEY_RARROW      67
#define KEY_LARROW      68
#define KEY_MWHEELDN    65
#define KEY_MWHEELUP    66

#define SYM_FILL        219

#define XSCALE          10
#define YSCALE          7  
#define HAND_INACTIVE_CARD_WIDTH 2

#define Y_GRID_OFFSET   2
#define Y_HAND_OFFSET   1

class NCurses_commander : public Commander
{
    private: 
    class UI_Object;
    typedef std::reference_wrapper<UI_Object> UIobj_ref;

    class Rect;
    class Text_box;
    class Scroll_box;
    class Unit_sprite;
    class Card_sprite;

    public:
    inline const bool is_on() { return on;}
    int x_scale, y_scale;

    NCurses_commander();
    ~NCurses_commander();

    Order get_order() override;
    void process_event(Event event) override;
    void apply_updates();

    private:
    bool on;
    int x_term_size, y_term_size; //getmaxyx(stdscr, height, width);

    class UI_Object
    {
        public:
        int x = 0, y = 0;  
        bool use_absolute_position = false;
        bool visible = true;
        std::vector<UIobj_ref> children = {};

        void draw(int orig_y = 0, int orig_x = 0);

        private:
        virtual void draw_self(int orig_y = 0, int orig_x = 0) {}
    };
    class Rect : public UI_Object
    {
        public:
        int width = 0, height = 0;

        int border_color = CPAIR_INVERTED, fill_color = CPAIR_INVERTED;
        unsigned tl_corner = ' ', tr_corner = ' ', bl_corner = ' ', br_corner = ' ';
        unsigned t_border = ' ', b_border = ' ', l_border = ' ', r_border = ' ';
        unsigned fill = ' ';
        bool draw_filled = false;

        inline void set_color(int color);
        inline void set_corners(unsigned symbol);
        inline void set_hborders(unsigned symbol);
        inline void set_vborders(unsigned symbol);
        inline void set_borders(unsigned symbol);
        inline void set_all(unsigned symbol);

        private:
        virtual void draw_self(int orig_y, int orig_x) override;
    };
    class Text_box : public UI_Object
    {
        public:
        Text_box(std::string txt = "") : text(txt) {}
        int color = CPAIR_NORMAL;
        int width = 0, height = 0;
        std::string text;

        private:
        virtual void draw_self(int orig_y, int orig_x) override;
    };
    class Scroll_box : public Text_box
    {
        public:
        bool from_bottom = false;

        private:
        virtual void draw_self(int orig_y, int orig_x) override;
    };
    class Unit_sprite : public UI_Object
    {
        public:
        Unit_sprite(Card_info c_info = {});
        void set_card(Card_info c_info);
        const inline Card_info get_card() { return card_info;}
        
        int x_scale = XSCALE, y_scale = YSCALE;
        bool darken_name;

        private:
        Card_info card_info;
        virtual void draw_self(int y, int x) override;

        Rect rect;
        Text_box name, value, advantage, indicator;
    };
    class Card_sprite : public UI_Object
    {
        public:
        Card_sprite(Description_generator::Card_descr c_descr = {});
        Card_sprite(Card_info c_info) : Card_sprite(Description_generator::get().get_card_instance(c_info.global_id)) { card_info = c_info;}
        Card_sprite(int id) : Card_sprite(Description_generator::get().get_card_instance(id)) {}

        void set_desc(Description_generator::Card_descr c_descr);
        void set_desc(int id) { set_desc(Description_generator::get().get_card_instance(id));};
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

        virtual void draw_self(int orig_y, int orig_x) override;

        Rect rect;
        Text_box name, cost, value, ability_text, flavor_text;
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

    Rect grid_cell;
    Rect grid_border;
    Rect grid_capture_area;
    Rect grid_highlight;
    Rect hand_highlight;

    Rect hand_cards_left, hand_cards_right;
    Text_box bottom_line;
    Text_box hand_tooltip_r, hand_tooltip_l;
    Text_box status_message;

    Order pending_order;
    bool order_is_ready;

    void render_UI();
    void render_hand();
    void render_grid();

    void change_focus_area(int focusarea);
    void change_grid_focus(int x, int y);

    unsigned get_input();
};