#pragma once

#include <ncurses.h>

#include <optional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>
#include <mutex>

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

#define KEY_UARROW      91
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
    private:
        TUI();

        TUI(const TUI&) = delete;
        TUI& operator=(const TUI&) = delete;

    public:
    
    static TUI& get();
    ~TUI();

    inline void clear() {
        ::erase();
    }
    inline void render() {
        ::refresh();
    }
    inline void set_color_pair(short id, short foreground, short background) { 
        init_pair(id, foreground, background);
    }
    unsigned get_input();

    class UI_Object;
    typedef std::reference_wrapper<UI_Object> UIobj_ref;
    class UI_Object
    {
        public:
        UI_Object() : tui(TUI::get()) {}

        int x = 0, y = 0;  
        bool use_absolute_position = false;
        bool visible = true;
        std::vector<UIobj_ref> children = {};

        void draw(unsigned input = 0, int orig_y = 0, int orig_x = 0);

        protected:
        TUI& tui;
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) {}
    };

    class Rect : public UI_Object
    {
        public:
        int width = 0, height = 0;

        int border_color = 0, fill_color = 0;
        unsigned tl_corner = ACS_ULCORNER, tr_corner = ACS_URCORNER, bl_corner = ACS_LLCORNER, br_corner = ACS_LRCORNER;
        unsigned t_border = ACS_HLINE, b_border = ACS_HLINE, l_border = ACS_VLINE, r_border = ACS_VLINE;
        unsigned fill = ' ';
        bool draw_filled = false;

        inline void set_color(int color) {
            fill_color   = color;
            border_color = color;
        }
        inline void set_hborders(unsigned symbol) {
            t_border = symbol;
            b_border = symbol;
        }
        inline void set_vborders(unsigned symbol) {
            l_border = symbol;
            r_border = symbol;
        }
        inline void set_borders(unsigned symbol) {
            t_border = symbol;
            b_border = symbol;
            l_border = symbol;
            r_border = symbol;
        }
        inline void set_corners(unsigned symbol) {
            tl_corner = symbol;
            tr_corner = symbol;
            bl_corner = symbol;
            br_corner = symbol;
        } 
        inline void set_all(unsigned symbol) {
            set_borders(symbol);
            set_corners(symbol);
            fill = symbol;
        } 

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Text_box : public UI_Object
    {
        public:
        Text_box(std::string txt = "") : text(txt) {}
        int color = 0;
        int width = 0, height = 0;
        std::string text;

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Scroll_box : public Text_box
    {
        public:

        Focus focus;
        int scroll_pos = 0;
        bool from_bottom = false;

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Input_box : public Text_box
    {
        public:
        Focus focus;
        
        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };
};