#include "Game_master.hpp"

#include <stdexcept>
#include <iostream>

#include "Misc_functions.hpp"
#include "Ability.hpp"
class Game_master;

Game_master::Card::Card(Game_master& master, int id, int oid) : entity_id(), card_id(id), owner_id(oid), controller_id(-1)
{
    status = CSTATUS_UNDEFINED;
    
    // Initialize abilities
    switch (card_id)
    {
        case BOUNTYHUNTER:
        case TERRORGUARD:
        case MEGALO:
        case MAIMDROID:
        case OMNITANK:
        case PEPTANE:
        case MACHINESHOP:
        case FISSION:
        case DEMOLITIONIST:
        break;

        case UNICYCLONE:
        {
            std::unique_ptr<Ability_simple> ability = std::make_unique<Ability_simple>(master, std::ref(*this), std::pair<Cause_ref, Effect>
                                                                    (std::ref(leaves_play), 
                                                                    [this, &master](EArgs _){ master.players[this->controller_id].funds += 2;}));
            abilities.push_back(std::move(ability));
        }
        break;
        case LOGISTICS:
        {
            std::unique_ptr<Ability_simple> ability = std::make_unique<Ability_simple>(master, std::ref(*this), std::pair<Cause_ref, Effect>
                                                                    (std::ref(enters_play), 
                                                                    [this, &master](EArgs _){ master.resolve_draw_multi(this->controller_id, 3);}));
            abilities.push_back(std::move(ability));
        }
        break;
    }
    #ifdef LOGGER_ON
        Logger::get().write("Created card instance " + ref_to_string(this));
        Logger::get().write("(EID: " + std::to_string(entity_id) + " CID: " + std::to_string(id) + ")");
    #endif

    reset();
}

void Game_master::Card::reset() 
{
    #ifdef LOGGER_ON
        Logger::get().write("Resetting card " + ref_to_string(this));
        Logger::get().write("(EID: " + std::to_string(entity_id) + " CID: " + std::to_string(card_id) + ", Status: " + std::to_string(status) + ")");
    #endif

    can_attack = false;
    can_move = false;
    is_overwhelmed = false;
    x = -1, y = -1;

    switch (card_id)
    {
    case BOUNTYHUNTER:
        type = CTYPE_UNIT;

        cost = 1;
        value = 2;
        break;
    case TERRORGUARD:
        type = CTYPE_UNIT;

        cost = 3;
        value = 5;
        break;
    case MEGALO:
        type = CTYPE_UNIT;

        cost = 5;
        value = 8;
        break;
    case MAIMDROID:
        type = CTYPE_UNIT;

        cost = 3;
        value = 3;
        advantage = 1;
        break;
    case OMNITANK:
        type = CTYPE_UNIT;

        cost = 6;
        value = 7;
        advantage = 1;
        break;
    case DEMOLITIONIST:
        type = CTYPE_UNIT;

        cost = 2;
        value = 6;
        advantage = -1;
        break;
    case PEPTANE:
        type = CTYPE_CONTRACT;

        cost = 2;
        value = 3; 
        break;
    case MACHINESHOP:
        type = CTYPE_CONTRACT;

        cost = 4;
        value = 6;
        break;
    case FISSION:
        type = CTYPE_CONTRACT;

        cost = 6;
        value = 9;
        break;
    case UNICYCLONE:
        type = CTYPE_UNIT;

        cost = 3;
        value = 3;
        break;
    case LOGISTICS:
        type = CTYPE_TACTIC;

        cost = 1;
    }
}

Commander::Card_info Game_master::Card::get_info()
{
    Commander::Card_info info;

    info.owner_id = owner_id;
    info.card_id = card_id;
    info.entity_id = entity_id;

    info.x = x, info.y = y;

    info.value = value;
    info.cost = cost;
    info.advantage = advantage;
    info.type = type;

    info.can_attack = can_attack;
    info.can_move = can_move;
    info.is_overwhelmed = is_overwhelmed;

    return info;
}