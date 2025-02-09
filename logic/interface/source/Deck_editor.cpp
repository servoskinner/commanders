#include "Deck_editor.hpp"

void Deck_editor::on_load()
{
    for(int id = 0; id < decks.size(); id++) {
        load_deck(id);
    }
}

void Deck_editor::draw_self(unsigned input = 0, int orig_y = 0, int orig_x = 0)
{
    switch (screen)
    {
    case CHOOSE_DECK:
        {
            switch (input)
            {
            case 'a': case 'A':
                selected_deck = (selected_deck - 1) % decks.size();
                break;
            
            case 'd': case 'D':
                selected_deck = (selected_deck + 1) % decks.size();
                break;

            case KEY_SUBMIT:
                screen = EDIT_DECK;
            }
            for (int id = 0; id < decks.size(); id++)
            {
                TUI::Rect deck_visualizer(46, 28);
                deck_visualizer.set_border_color(COLOR_BRIGHT_BLACK);

                if (decks[id].has_value()) {
                    TUI::Text deck_name(decks[id]->name, 0, 24);
                    deck_name.foreground = COLOR_BRIGHT_BLACK;
                    deck_name.draw(0, 15, 4 + 29*id);

                    TUI::Text stats("cards: " + std::to_string(decks[id]->cards.size()));
                    stats.foreground = COLOR_BRIGHT_BLACK;
                    stats.draw(0, 44, 4 + 29*id);
                }
                else {
                    deck_visualizer.set_all({'.', COLOR_BRIGHT_BLACK});
                    TUI::Text empty("EMPTY");
                    empty.foreground = COLOR_BRIGHT_BLACK;
                    empty.draw(0, 15, 14+29*id);
                }
                deck_visualizer.draw(0, 2, 2 + 29*id);
            }
            TUI::Rect choice(46, 28);
            choice.set_border_color(COLOR_BRIGHT_CYAN);
            choice.draw(0, 2, 2 + 29*selected_deck);
            break;
        }
    
    case EDIT_DECK:
        {
            
            break;
        }
    }
}

void Deck_editor::load_deck(int id)
{
    Archivist archivist("decks");
    std::string name_locator = std::to_string(id) + std::to_string(Deck_editor::NAME);
    std::string roster_locator = std::to_string(id) + std::to_string(Deck_editor::ROSTER);

    std::optional<std::string> name = archivist.get_string(name_locator);
    std::optional<std::vector<unsigned short>> roster = archivist.get_vector<unsigned short>(roster_locator);

    if(!name.has_value() || !roster.has_value()) {
        decks[id] = {};
    }
    else {
        decks[id] = {{name.value(), roster.value()}};
    } 
}

void Deck_editor::save_deck(int id)
{
    Archivist archivist("decks");
    std::string name_locator = std::to_string(id) + std::to_string(Deck_editor::NAME);
    std::string roster_locator = std::to_string(id) + std::to_string(Deck_editor::ROSTER);

    if(decks[id].has_value())
    {
        archivist.put_string(name_locator, decks[id]->name);
        archivist.put_vector(roster_locator, decks[id]->cards);
    }
    else {
        archivist.del(name_locator);
        archivist.del(roster_locator);
    }
}

void Deck_editor::erase_deck(int id)
{
    decks[id] = {};
    Archivist archivist("decks");
    std::string name_locator = std::to_string(id) + std::to_string(Deck_editor::NAME);
    std::string roster_locator = std::to_string(id) + std::to_string(Deck_editor::ROSTER);

    archivist.del(name_locator);
    archivist.del(roster_locator);
}