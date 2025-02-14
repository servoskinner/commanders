#pragma once

#include <map>
#include <functional>
#include <utility>
#include <optional>
#include <list>

#include "Game_master.hpp"
#include "Unique.hpp"

/// @brief Represents a collection of trigger-event pairs and memory related to them associated with a card.
class Game_master::Ability
{
    protected:
    Unique entity_id;

    public:


    Ability() : entity_id() {} 
    virtual ~Ability();
};