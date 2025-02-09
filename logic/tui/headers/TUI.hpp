#pragma once

#include <optional>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <string>
#include <array>

#include <ncurses.h>

#include "Vector2i.hpp"

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
#define KEY_SUBMIT      10

#define KEY_UARROW      91
#define KEY_DARROW      66
#define KEY_RARROW      67
#define KEY_LARROW      68
#define KEY_MWHEELDN    65
#define KEY_MWHEELUP    66

#define SYM_FILL        219

/// @brief The singleton that manages Text-based User Interface.
class TUI
{
    public: 
    enum Color : unsigned short {
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHTBLACK,
    BRIGHTRED,
    BRIGHTGREEN,
    BRIGHTYELLOW,
    BRIGHTBLUE,
    BRIGHTMAGENTA,
    BRIGHTCYAN,
    BRIGHTWHITE,

    TRANSPARENT = 0xFFFFU
    };

    /// @brief A single terminal symbol.
    struct Glyph
    {
        unsigned int symbol; 
        Color foreground;
        Color background;

        constexpr Glyph(unsigned short symbol = ' ', Color foreground = WHITE, Color background = TRANSPARENT)
                       : symbol(symbol), foreground(foreground), background(background) {} 

        /// @brief Superimpose two symbols, ignoring the transparent parts.
        inline Glyph operator+(const Glyph& overlay) const {
            Glyph new_glyph = overlay;

            if (overlay.foreground == TRANSPARENT) {
                new_glyph.symbol = symbol;
                new_glyph.foreground = foreground;
            }
            if (overlay.background == TRANSPARENT) {
                new_glyph.background = background;
            }
            return new_glyph;
        }

        /// @brief Superimpose two symbols, ignoring the transparent parts.
        inline Glyph& operator+=(const Glyph& overlay) {
            if (overlay.foreground != TRANSPARENT) {
                symbol = overlay.symbol;
                foreground = overlay.foreground;
            }
            if (overlay.background != TRANSPARENT) {
                background = overlay.background;
            }
            return *this;
        }

        inline bool operator==(const Glyph& other) const 
            { return symbol == other.symbol && background == other.background && foreground == other.foreground; }

        inline bool operator!=(const Glyph& other) const 
            { return symbol != other.symbol || background != other.background || foreground != other.foreground; }
    };
        
    TUI() = default;
    virtual ~TUI() = default;

    /// @return Dimensions of terminal window.
    virtual Vector2i get_screen_size() = 0;

    /// @return Code of last key pressed or 0 if there is none.
    virtual unsigned get_input() { return 0; };

    /// @brief Draw differences and synchronize screen buffer.
    void render();

    /// @return Terminal center coordinates.
    inline Vector2i get_center() {
        Vector2i size = get_screen_size();
        return size / 2;
    }

    /// @brief forcibly invalidates screen buffer.
    void clear();
    
    /// @brief Places glyph in screen buffer at given coordinates.
    void draw_glyph(Vector2i pos, Glyph glyph);

    /// @brief Draws a rectangle with given limits
    /// @param bottom_left lower values of both coordinates (included)
    /// @param top_right upper values of both coordinates (excluded)
    /// @param fill Symbol to fill the rect with
    void draw_rect(Vector2i bottom_left, Vector2i top_right, Glyph fill);

    /// @brief 
    /// @param text Text to print
    /// @param y Vertial origin
    /// @param x Horizontal origin
    /// @param 
    void print(std::string text, Vector2i pos, Color fore = WHITE, Color back = TRANSPARENT);

    class UI_Object;
    typedef std::shared_ptr<UI_Object> UIobj_ptr;

    protected:
        enum Buffer_layer {
            OLD,
            NEW
        };

        /// @brief Used to compare and draw only characters that have been modified.
        /// [0] is the "old" flushed buffer and [1] is the modified "new" one.
        std::vector<std::vector<std::array<Glyph, 2>>> screen_buffer {};

        /// @brief Places a glyph that has been changed on the real screen.
        virtual void output_glyph(Vector2i pos, Glyph glyph) = 0;

        /// @brief Something that should happen before the screen is updated.
        virtual void pre_render() {}

        /// @brief Something that should happen after the screen is updated.
        virtual void post_render() {}

    private:
        TUI(const TUI&) = delete;
        TUI(const TUI&&) = delete;
        TUI& operator=(const TUI&) = delete;
        TUI& operator=(const TUI&&) = delete;
};