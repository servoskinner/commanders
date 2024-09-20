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
    Card& attached_to;
    Game_master& master;

    /// @brief  Something that should happen, represented as cause-effect pair.
    class Binding
    {
        private:
        Cause& trig; /// The happening that triggers the effect.
        Unique key; /// Unique ID used to decouple the binding from whatever fires the event.

        public:
        Binding(Cause& trigger, Effect reaction);
        ~Binding();
    };

    public:
    Unique entity_id;
    Ability(Game_master& gm, Card& card) : master(gm), attached_to(card) {} 
    ~Ability() = default;
};

/// @brief Single Cause-Effect pair with no memory.
class Game_master::Ability_simple : public Game_master::Ability
{
    public:
    Ability_simple(Game_master& gm, Card& card, std::pair<Cause_ref, Effect> effect) : Ability(gm, card), binding(effect.first, effect.second) {}

    private:
    Binding binding;
};