#pragma once

#include <map>
#include <functional>
#include <utility>
#include <optional>

#include "Game_master.hpp"
#include "Unique.hpp"



class Game_master::Ability
{
    private:
    Card& attached_to;

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
    Ability(Card& card) : attached_to(card) {} 

    template <int amount>
    void draw_cards(Game_master& master, std::vector<int> args)
    {
        for (int i = 0; i < amount; i++) {
            master.resolve_draw(attached_to.owner_id);
        }
    }

    template <int amount>
    void gain_credits(Game_master& master, std::vector<int> args)
    {
        master.players[attached_to.owner_id].funds += amount;
    }
};