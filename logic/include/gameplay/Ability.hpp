#pragma once

#include <map>
#include <functional>
#include <utility>
#include <optional>

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

    template <int amount>
    void draw_cards(std::vector<int> args)
    {
        for (int i = 0; i < amount; i++) {
            master.resolve_draw(attached_to.owner_id);
        }
    }

    template <int amount>
    void gain_credits(std::vector<int> args)
    {
        master.players[attached_to.owner_id].funds += amount;
    }
};

class Game_master::Ability_simple : Game_master::Ability
{
    public:
    Ability_simple(Game_master& gm, Card& card, std::vector<std::pair<Trigger&, Reaction>> effects);

    protected:
    std::vector<Binding> bindings;
};