#include "Game_master.hpp"
#include "Ability.hpp"

Game_master::Ability::Binding::Binding(Game_master::Trigger& trigger, Game_master::Reaction reaction) : trig(trigger), key()
{
    trig.insert({key.get_id(), reaction});
}

Game_master::Ability::Binding::~Binding()
{
    trig.erase(key.get_id());
}

Game_master::Ability_simple::Ability_simple(Game_master& gm, Card& card, std::vector<std::pair<Trigger&, Reaction>> effects) 
                                           : Ability(gm, card), bindings()
{
    for (std::pair<Trigger&, Reaction>& effect : effects) {
        bindings.emplace_back(effect.first, effect.second);
    }
};