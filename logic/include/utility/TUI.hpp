#pragma once

#include <ncurses.h>

#include <optional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <array>
#include <mutex>

#define COLOR_BRIGHT_BLACK		8
#define COLOR_BRIGHT_RED		9
#define COLOR_BRIGHT_GREEN		10
#define COLOR_BRIGHT_YELLOW		11
#define COLOR_BRIGHT_BLUE		12
#define COLOR_BRIGHT_MAGENTA	13
#define COLOR_BRIGHT_CYAN		14
#define COLOR_BRIGHT_WHITE		15

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
        erase();
    }
    inline void render() {
        refresh();
    }
    inline void set_color_pair(short id, short foreground, short background) { 
        init_pair(id, foreground, background);
    }
    unsigned get_input();

    struct Glyph
    {
        unsigned symbol = ' ';
        short color = 0;
    };

    inline void draw_glyph(int y, int x, Glyph glyph) {
        attron(COLOR_PAIR(glyph.color));
        mvaddch(y, x, glyph.symbol);
        attroff(COLOR_PAIR(glyph.color));
    }

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

        Glyph tl_corner = {ACS_ULCORNER}, tr_corner = {ACS_URCORNER}, bl_corner = {ACS_LLCORNER}, br_corner = {ACS_LRCORNER};
        Glyph t_border = {ACS_HLINE}, b_border = {ACS_HLINE}, l_border = {ACS_VLINE}, r_border = {ACS_VLINE};
        Glyph fill = {};

        bool draw_filled = false;

        inline void set_border_color(short color) {
            tl_corner.color = color;
            tr_corner.color = color;
            bl_corner.color = color;
            br_corner.color = color;
            t_border.color = color;
            b_border.color = color;
            l_border.color = color;
            r_border.color = color;
        }
        inline void set_hborders(Glyph glyph) {
            t_border = glyph;
            b_border = glyph;
        }
        inline void set_vborders(Glyph glyph) {
            l_border = glyph;
            r_border = glyph;
        }
        inline void set_borders(Glyph glyph) {
            set_hborders(glyph);
            set_vborders(glyph);       
        }
        inline void set_corners(Glyph glyph) {
            tl_corner = glyph;
            tr_corner = glyph;
            bl_corner = glyph;
            br_corner = glyph;
        } 
        inline void set_all(Glyph glyph) {
            set_borders(glyph);
            set_corners(glyph);
            fill = glyph;
        } 

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Sprite : public UI_Object
    {
        public:
        Sprite(int width, int height);

        inline std::pair<int, int> get_size() { return {sprite.size(), sprite[0].size()};};
        inline Glyph get_glyph(int y, int x) { return sprite[y][x];};
        inline void set_glyph(int y, int x, Glyph glyph) { sprite[y][x] = glyph;};
        void set_all(Glyph glyph);

        private:
        std::vector<std::vector<Glyph>> sprite;
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Text : public UI_Object
    {
        public:
        Text(std::string txt = "") : text(txt) {}
        short color = 0;
        int width = 0, height = 0;
        std::string text;

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Scrollable_text : public Text
    {
        public:
        int scroll_pos = 0;
        bool from_bottom = false;

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Text_input : public Text
    {
        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };
};