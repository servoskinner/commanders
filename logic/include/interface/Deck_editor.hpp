#include "TUI.hpp"
#include "Animations.hpp"
#include "Archivist.hpp"
#include "Timer.hpp"
#include "Description_generator.hpp"
#include <string>

class Deck_editor : public TUI::UI_Object
{
    public:
    enum Deck_editor_screen
    {
        CHOOSE_DECK,
        EDIT_DECK
    };
    Deck_editor_screen screen = CHOOSE_DECK;

    struct Deck_file {
        std::string name;
        std::vector<unsigned short> cards;
    };

    enum Deck_locator_suffixes {
        NAME,
        ROSTER
    };

    int selected_deck = 0;

    std::array<std::optional<Deck_file>, 4> decks;
    bool prompt_brought_up;

    void on_load();
    void on_exit();

    void load_deck(int id);
    void save_deck(int id);
    void erase_deck(int id);

    void draw_self(unsigned input, int orig_y, int orig_x) override;
};