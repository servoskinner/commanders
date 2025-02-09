#include "TUI.hpp"
#include "Misc_functions.hpp"


void TUI::render() {

    pre_render();
    // Get new screen size and resize the buffer:
    Vector2i screen_size = get_screen_size();

    // Trim Y-axis first to avoid resizing the last lines
    // that are about to be deleted if X was changed too.
    if (screen_size.y != screen_buffer.size()) {
        screen_buffer.resize(screen_size.y, std::vector<std::array<Glyph, 2>>(screen_size.x));
    }
    // Then resize remaining rows to appropriate width
    for (auto& row : screen_buffer) {
        if (row.size() == screen_size.x) {
            break;
        }
        row.resize(screen_size.x);
    }

    // Report and propagate changed characters
    for (int y = 0; y < screen_buffer.size(); y++) {
        auto& row = screen_buffer[y];
        for(int x = 0; x < row.size(); x++) {
            
            if (row[x][OLD] != row[x][NEW]) {
                output_glyph({x, y}, row[x][NEW]);
                row[x][OLD] = row[x][NEW];
            }
        }
    }

    post_render();
}

void TUI::clear() {
    for (auto& line : screen_buffer) {
        for (auto& pos : line) {
            pos[NEW] = {' ', TRANSPARENT, TRANSPARENT};
        }
    }
}
    
void TUI::draw_glyph(Vector2i pos, Glyph glyph) {
    // Check bounds
    if (pos >= Vector2i(0, 0) && pos.y < screen_buffer.size() && pos.x < screen_buffer[pos.y].size()) {
        screen_buffer[pos.y][pos.x][NEW] += glyph;
    }
}


void TUI::draw_rect(Vector2i bottom_left, Vector2i top_right, Glyph fill) {
    if (screen_buffer.empty() || !(bottom_left < top_right)) {
        // Screen size is 0 in at least one dimension
        // Or bounds are in wrong order
        return;
    }
    Vector2i buffer_size {(int)screen_buffer[0].size(), (int)screen_buffer.size()};
    if (!(bottom_left < buffer_size)) { // Rect is out of bounds
        return;
    }

    // Trim to fit within screen
    bottom_left = bottom_left.clamp(buffer_size + Vector2i(1, 1));
    top_right = top_right.clamp(buffer_size + Vector2i(1, 1));

    for (int y = bottom_left.y; y < top_right.y; y++) {
        auto& row = screen_buffer[y];
        for (int x = bottom_left.x; x < top_right.x; x++) {
            row[x][NEW] += fill;
        }
    }
}

void TUI::print(std::string text, Vector2i pos, Color fore, Color back) {
    if (screen_buffer.empty()) { // Screen size is 0x0
        return;
    }
    Vector2i buffer_size {(int)screen_buffer[0].size(), (int)screen_buffer.size()};
    if (!(pos < buffer_size)) { // Attempting to draw out of bounds
        return;
    }

    // Iterate over rows and columns
    auto line_iter = screen_buffer.begin() + pos.y;
    auto symbol_iter = line_iter->begin() + pos.x;

    for(char ch : text) {
        if (ch == '\n') { // Jump to next line
            line_iter++;
            if (line_iter == screen_buffer.end()) {
                break;
            }
            symbol_iter = line_iter->begin() + pos.x;
        }
        else {
            if (symbol_iter != line_iter->end()) { // Put char if there is space to write
                (*symbol_iter)[NEW] += {(unsigned short)ch, fore, back};
                symbol_iter++;
            } // Otherwise, discard symbols until string ends or a newline is found
        }
    }
}