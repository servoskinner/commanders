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

typedef unsigned char Color;

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

    inline std::pair<int, int> get_size() { 
        return {getmaxy(stdscr), getmaxx(stdscr)};
    }

    inline std::pair<int, int> get_center() { 
        return {getmaxy(stdscr)/2, getmaxx(stdscr)/2};
    }

    unsigned get_input();

    inline static unsigned short get_color_code(Color foreground = COLOR_WHITE, Color background = COLOR_BLACK)
    {
        if (foreground == COLOR_WHITE && background == COLOR_BLACK) {
            return 0;
        }
        return background * 16 + foreground;
    }

    struct Glyph
    {
        unsigned symbol = ' '; 
        Color foreground = COLOR_WHITE;
        Color background = COLOR_BLACK;
    };
    
    inline void draw_glyph(int y, int x, Glyph glyph) {
        attron(COLOR_PAIR(get_color_code(glyph.foreground, glyph.background)));
        mvaddch(y, x, glyph.symbol);
        attroff(COLOR_PAIR(get_color_code(glyph.foreground, glyph.background)));
    }

    class UI_Object;
    typedef std::reference_wrapper<UI_Object> UIobj_ref;
    class UI_Object
    {
        public:
        UI_Object(int y = 0, int x = 0) : x(x), y(y), tui(TUI::get()) {}
        UI_Object& operator= (const UI_Object& other) = default;

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
        Rect(int height = 1, int width = 1, int y = 0, int x = 0, 
             Color foreground = COLOR_WHITE, Color background = COLOR_BLACK) : height(height), width(width), UI_Object(y, x)
            { set_border_color(foreground, background);}
        Rect& operator= (const Rect& other) = default;

        int width = 0, height = 0;

        Glyph tl_corner = {ACS_ULCORNER}, tr_corner = {ACS_URCORNER}, bl_corner = {ACS_LLCORNER}, br_corner = {ACS_LRCORNER};
        Glyph t_border = {ACS_HLINE}, b_border = {ACS_HLINE}, l_border = {ACS_VLINE}, r_border = {ACS_VLINE};
        Glyph fill = {};

        bool draw_filled = false;

        inline void set_border_color(Color foreground = COLOR_WHITE, Color background = COLOR_BLACK) {
            tl_corner.foreground = foreground;
            tr_corner.foreground = foreground;
            bl_corner.foreground = foreground;
            br_corner.foreground = foreground;
            t_border.foreground = foreground;
            b_border.foreground = foreground;
            l_border.foreground = foreground;
            r_border.foreground = foreground;

            tl_corner.background = background;
            tr_corner.background = background;
            bl_corner.background = background;
            br_corner.background = background;
            t_border.background = background;
            b_border.background = background;
            l_border.background = background;
            r_border.background = background;
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
        Sprite(int height, int width, int y = 0, int x = 0);
        Sprite& operator=(Sprite &other) { sprite = other.sprite; return *this;};
        Sprite(const Sprite& other) = default;

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
        Text(std::string text = "", int height = 0, int width = 0, int y = 0, int x = 0, 
             Color foreground = COLOR_WHITE, Color background = COLOR_BLACK)
         : text(text), height(height), width(width), background(background), foreground(foreground), UI_Object(y, x) {}
        Text& operator= (const Text& other) = default;

        Color background = COLOR_BLACK;
        Color foreground = COLOR_WHITE;
        int width = 0, height = 0;
        std::string text;

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Scrollable_text : public Text
    {
        public:
        Scrollable_text(std::string text = "", int height = 0, int width = 0, int y = 0, int x = 0,
                        Color foreground = COLOR_WHITE, Color background = COLOR_BLACK)
         : Text(text, height, width, y, x, foreground, background) {}
        Scrollable_text& operator= (const Scrollable_text& other) = default;

        int scroll_pos = 0;
        bool from_bottom = false;

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };

    class Text_input : public Text
    {
        public:
        Text_input(int height = -1, int width = -1, int y = 0, int x = 0,
                   Color foreground = COLOR_WHITE, Color background = COLOR_BLACK)
         : Text("", height, width, y, x, foreground, background) {}
        Text_input& operator= (const Text_input& other) = default;

        protected:
        virtual void draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0) override;
    };
};