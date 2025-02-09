#include "NCursesTUI.hpp"
#include "Misc_functions.hpp"

#include <iostream>

std::mutex NCursesTUI::ncurses_mutex {};
std::optional<std::lock_guard<std::mutex>> NCursesTUI::ncurses_mtx_lock {};

TUI& NCursesTUI::get()
{
    static NCursesTUI instance;
    return instance;
}

NCursesTUI::NCursesTUI() 
{
    ncurses_mtx_lock.emplace(ncurses_mutex);

    initscr();

    raw();                  // No line buffering for keys
    noecho();               // Do not echo characters in terminal
    curs_set(0);            // Hide cursor   

    start_color(); 
    // init color pairs
    for(unsigned char fore = 0; fore < 16; fore++) {
        for(unsigned char back = 0; back < 16; back++) {
            set_color_pair(get_color_code((Color)fore, (Color)back), fore, back);
        }
    }
    ncurses_mtx_lock.reset();
}

NCursesTUI::~NCursesTUI() 
{
    endwin();
}

unsigned short NCursesTUI::get_color_code(Color foreground, Color background) const
{
    // Remove transparent parts
    if (background == TRANSPARENT) {
        background = default_background == TRANSPARENT ? BLACK : default_background;
    }
    if (foreground == TRANSPARENT) {
        foreground = background;
    }
    // (WHITE, BLACK) and (BLACK, BLACK) are swapped
    if (background == BLACK) {
        if (foreground == WHITE) {
            return 0U;
        }
        if (foreground == BLACK) {
            return (unsigned short)BLACK * 16 + (unsigned short)WHITE;
        }
    }
    // All other colors are mapped normally
    return (unsigned short)background * 16 + (unsigned short)foreground;
}

void NCursesTUI::output_glyph(Vector2i pos, Glyph glyph) {
    attron(COLOR_PAIR(get_color_code(glyph.foreground, glyph.background)));
    mvaddch(pos.y, pos.x, glyph.symbol);
    attroff(COLOR_PAIR(get_color_code(glyph.foreground, glyph.background)));
}

void NCursesTUI::pre_render() { 
    ncurses_mtx_lock.emplace(ncurses_mutex); 
    // Read input
    timeout(0);
    last_input = getch();
    if (last_input == ERR) {
        last_input = 0;
    }
}
void NCursesTUI::post_render() { 
    ncurses_mtx_lock.reset(); 
}

unsigned NCursesTUI::get_input()
{
    return last_input;
}

void NCursesTUI::make_pretty(Rect& rect) {
    rect.tl_corner.symbol = ACS_ULCORNER;
    rect.tr_corner.symbol = ACS_URCORNER;
    rect.bl_corner.symbol = ACS_LLCORNER;
    rect.br_corner.symbol = ACS_LRCORNER;

    rect.t_border.symbol = ACS_HLINE;
    rect.r_border.symbol = ACS_VLINE;
    rect.b_border.symbol = ACS_HLINE;
    rect.l_border.symbol = ACS_VLINE;
}
