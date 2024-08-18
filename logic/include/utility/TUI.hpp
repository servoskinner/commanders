#pragma once

#include <ncurses.h>

#include <optional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>
#include <mutex>
#include <memory>

#include "Focus.hpp"

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

class Rect;
class Text_box;
class Scroll_box;
class Unit_sprite;
class Card_sprite;

class TUI
{
    public:
    
    static TUI& invoke();

    class UI_Object;
    typedef std::reference_wrapper<UI_Object> UIobj_ref;
    class UI_Object
    {
        public:
        UI_Object() : tui(TUI::invoke()) {}

        int x = 0, y = 0;  
        bool use_absolute_position = false;
        bool visible = true;
        std::vector<UIobj_ref> children = {};

        void draw(int orig_y = 0, int orig_x = 0);

        protected:
        TUI& tui;
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

        protected:
        virtual void draw_self(int orig_y, int orig_x) override;
    };

    class Text_box : public UI_Object
    {
        public:
        Text_box(std::string txt = "") : text(txt) {}
        int color = CPAIR_NORMAL;
        int width = 0, height = 0;
        std::string text;

        protected:
        virtual void draw_self(int orig_y, int orig_x) override;
    };

    class Scroll_box : public Text_box
    {
        public:
        Focus focus;
        bool from_bottom = false;

        protected:
        virtual void draw_self(int orig_y, int orig_x) override;
    };

    private:
        TUI();

        TUI(const TUI&) = delete;
        TUI& operator=(const TUI&) = delete;
};