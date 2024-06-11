#pragma once

#include "Commander.hpp"
#include "Description_generator.hpp"
#include "Misc_functions.hpp"

#include <ncurses.h>
#include <iostream>
#include <utility>
#include <vector>
#include <string>

#define DEFAULT_UI_W_SCALE 5
#define DEFAULT_UI_H_SCALE 2

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
#define CPAIR_CYAN_HIGHLT       4
#define CPAIR_RED_HIGHLT        5
#define CPAIR_MAGENTA_HIGHLT    6
#define CPAIR_YELLOW            7

#define KEY_ESC         27
#define KEY_TAB         9
#define KEY_ENTR        10

#define KEY_UARROW      65
#define KEY_DARROW      66
#define KEY_RARROW      67
#define KEY_LARROW      68
#define KEY_MWHEELDN    65
#define KEY_MWHEELUP    66

#define Y_GRID_OFFSET 2
#define X_GRID_OFFSET 16 

class NCurses_commander : public Commander
{
    public:
    int x_scale, y_scale;

    Order get_order() override;
    void process_event(const Event& event) override;
    void apply_updates();

    NCurses_commander();
    ~NCurses_commander();

    private:
    int x_term_size, y_term_size; //getmaxyx(stdscr, height, width);

    enum focus_areas {
        HAND,
        FIELD,

    };
    int focus_x, focus_y, card_

    class UI_Object;
    typedef std::reference_wrapper<UI_Object> UIobj_ref;

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

        int border_color = 0, fill_color = 0;
        unsigned tl_corner = ' ', tr_corner = ' ', bl_corner = ' ', br_corner = ' ';
        unsigned h_border = ' ', v_border = ' ';
        unsigned fill = ' ';
        bool draw_filled = false;

        inline void set_color(int color);
        inline void set_corners(unsigned symbol);
        inline void set_borders(unsigned symbol);

        private:
        virtual void draw_self(int y, int x) override;
    };
    class Text_box : public UI_Object
    {
        public:
        int color = 0;
        int width = 0, height = 0;
        std::string text = "";

        private:
        virtual void draw_self(int y, int x) override;
    };
    // class Card_sprite : public UI_Object
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
    // class Card_sprite_extended : public UI_Object
    // {
    //     public:
    //     int x_scale = 18, y_scale = 7;

    //     Description_generator::Card_descr card_descr;
    //     //...
    //     private:
    //     virtual void draw_self(int y, int x) override;
    // };

    void render_UI();
    void draw_card(const Card_info& c_info);
    void draw_card_extended(int y, int x, const Card_info& c_info);

    int get_input();
};