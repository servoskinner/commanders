#include "Sprite_storage.hpp"
#include "Sprite.hpp"

bool save_sprite(TUI::Sprite& sprite, std::string filename)
{
    Archivist storage(filename);

    if (!storage.put("height", sprite.get_size().y) || !storage.put("width", sprite.get_size().x)) {
        return false;
    }

    int glyph_map_size = sprite.get_size().y * sprite.get_size().x;
    std::vector<TUI::Glyph> glyph_map(glyph_map_size);

    for(int y = 0; y < sprite.get_size().y; y++) {
            for(int x = 0; x < sprite.get_size().x; x++) {
                glyph_map[flatten_index({y, x}, sprite.get_size().x)] = sprite.get_glyph({x, y});
            }
        }    

    return storage.put("sprite", glyph_map);
}

std::optional<TUI::Sprite> load_sprite(std::string filename)
{
    Archivist storage(filename);
    
    std::optional<int> width_opt  = storage.get<int>("width");
    std::optional<int> height_opt = storage.get<int>("height");
    std::optional<std::vector<TUI::Glyph>> sprite_opt = storage.get<std::vector<TUI::Glyph>>("sprite");

    if (!width_opt.has_value()  || 
        !height_opt.has_value() ||
        !sprite_opt.has_value()) {
        return {};
    }

    int width = width_opt.value();
    int height = height_opt.value();

    TUI::Sprite sprite(height, width);

    std::vector<TUI::Glyph> glyph_map = sprite_opt.value();
    for(int y = 0; y < height; y++) {
            for(int x = 0; x < width; x++) {
                sprite.set_glyph({x, y}, glyph_map[flatten_index({y, x}, width)]);
            }
        }
    
    return {sprite};
}