#pragma once

#include <map>
#include <functional>
#include <utility>
#include <optional>
#include <list>

#include "Game_master.hpp"
#include "Unique.hpp"

class Game_master::Ability
{
    protected:
    Card& attached_to;
    Game_master& master;

    class Binding
    {
        private:
        Trigger& trig;
        Unique key;

        public:
        Binding(Trigger& trigger, Reaction reaction);
        ~Binding();
    };

    public:
    Unique entity_id;
    Ability(Game_master& gm, Card& card) : master(gm), attached_to(card) {} 
    ~Ability() = default;
};

class Game_master::Ability_simple : public Game_master::Ability
{
    public:
    Ability_simple(Game_master& gm, Card& card, std::pair<Trigger_ref, Reaction> effect) : Ability(gm, card), binding(effect.first, effect.second) {}

    private:
    Binding binding;
};