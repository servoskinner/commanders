#ifndef _INCLUDE_PLAYER_HPP
#define _INCLUDE_PLAYER_HPP

#include "Game_master.hpp"
#include "Ability.hpp"

#ifdef LOGGER_ON
#include "Logger.hpp"
#endif

class Game_master::Player
    {
    public: // _____________________________________________________________________________
        Player(int player_id, Deck& p_deck);

        // Movable
        Player(Player&& other) noexcept = default;
        Player& operator=(Player&& other) noexcept = default;

        const int id; /// Unique player ID and turn no. on which it issues orders.
        bool is_active; /// Whether the player is allowed to actively participate in game
        std::vector<Card> hand;
        Deck_ref deck;

        int points; /// Dominance Points scored by the player. a total of POINTS_REQ_FOR_VICTORY is required to win the game;
        int funds;  /// Funds used to play cards and fire abilities.

        /// @return Player information to display on interface, formatted and stripped of technical details.
        Player_info get_info(); 

        Cause turn_start;
        Cause turn_end;
        Cause deploys;
        Cause draws;

        private:
        // Non-copyable
        Player(Player& other) = delete;
        Player& operator=(Player& other) = delete;
    };

#endif