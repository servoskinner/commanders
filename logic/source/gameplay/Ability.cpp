#include "Game_master.hpp"
#include "Ability.hpp"

Game_master::Ability::Binding::Binding(Game_master::Cause& trigger, Game_master::Effect reaction) : trig(trigger), key()
{
    trig.insert({key.get_id(), reaction});
}

Game_master::Ability::Binding::~Binding()
{
    trig.erase(key.get_id());
}