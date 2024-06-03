#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>

#include "Commander.hpp"
#include "Card_index.hpp"
#include "Misc_functions.hpp"

#define GRID_WIDTH 		8
#define GRID_HEIGHT 	6

#define BASIC_INCOME 			1
#define BONUS_IF_ZERO_INCOME    1
#define STARTING_HAND_SIZE 		4
#define POINTS_REQ_FOR_VICTORY 	10


class Game_master
{
protected:

    class Card;
    class Deck;
    class Player;
    class Tile;
    class Card_generator;

    typedef std::reference_wrapper<Game_master::Player> player_ref;
    typedef std::reference_wrapper<Game_master::Deck>   deck_ref;
    typedef std::reference_wrapper<Game_master::Card>   card_ref;
    typedef std::reference_wrapper<Game_master::Tile>   tile_ref;

public: // _____________________________________________________________________________
    Game_master(const std::vector<pctrl_ref>& controllers, const std::vector<std::vector<int>> &deck_images);

    bool game_loop(); // Process player inputs and update status for everyone.
    void end_turn();  // Pass the turn to next player and process the necessary triggers.

    int get_turn() { return turn; }
    int get_absolute_turn() { return turn_absolute; }

    int get_grid_width() { return grid[0].size(); }
    int get_grid_height() { return grid.size(); }

protected: // _____________________________________________________________________________
    Card_generator &card_generator;

    std::vector<Player> players;
    std::vector<Deck> decks;
    std::vector<pctrl_ref> commanders; // entities that provide player inputs.
                                              // E.G. User, AIs, network-connected players
    // Playing field
    std::vector<std::vector<Tile>> grid; // The playing field. (0,0) is top left corner; X axis is vertical, Y is horizontal.
    std::vector<Game_master::card_ref> active_cards;   // Cards that are currently on the playing field.

    int exec_order(const Commander::Order &action);

    void update_status(int playerId);
    inline void broadcast_event(const Commander::Event& event);
    bool check_dominance(int playerId);

    int turn;
    int turn_absolute;

    bool game_is_on;
    // Actions
    bool deploy_card(Card &card, std::optional<tile_ref> targer);         // Place a card in play.
    bool resolve_movement(Card &card, const int &direction);   // Move a card in specified direction.
    void resolve_destruction(Card &card);                      // Remove a card from play and discard it.
    bool resolve_attack(Card &card, const int &direction); // Resolve an attack from one tile to another.

    int resolve_combat(Card &attacker, Card &defender); // Resolve combat between two units.
    enum combatOutcome
    {
        DEFENDER_DEAD,
        BOTH_DEAD,
        ATTACKER_DEAD
    };
    // Tiles
    std::vector<std::optional<Game_master::tile_ref>> get_4neighbors(const Tile &tile);
    std::vector<std::optional<Game_master::tile_ref>> get_8neighbors(const Tile &tile);
    // Players
    bool resolve_draw(int playerId);                             // Returns whether deck is empty or not
    bool discard(int playerId, int handIndex);                // Returns whether discarding this card was possible
    bool play_card(int playerId, int handIndex, std::optional<tile_ref> target); // Perform rule checks and deploy card
};

#include "Card.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "Tile.hpp"