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
};

class Game_master::Ability_simple : Game_master::Ability
{
    public:
    Ability_simple(Game_master& gm, Card& card, std::vector<std::pair<Trigger&, Reaction>> effects);

    protected:
    std::list<Binding> bindings;
};