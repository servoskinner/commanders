#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <cstring>

#include "Vector2i.hpp"
#include "Game_master.hpp"

class Commander;

typedef std::reference_wrapper<Commander> commander_ref;

/// @brief Generates standardized Orders from interface input and processes Events in the opposite direction.
class Commander // Acts as an interface between the Game Master and whoever is providing the player input.
{
public:

    std::vector<int> controlled_ids; // All players controlled by the commander.
    int active_id;    

    Game_master::Game_params game_params;
    Game_master::Game_state game_state;

    virtual std::optional<Game_master::Order> get_order() = 0;         
    virtual void process_event(Game_master::Event event) = 0;
    virtual void process_order_feedback(int code) = 0;

    virtual void update_state(Game_master::Game_state state) { game_state = state;}
    virtual void set_params(Game_master::Game_params params) { game_params = params;}
};