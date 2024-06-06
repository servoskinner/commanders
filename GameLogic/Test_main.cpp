#include "Card_index.hpp"
#include "CLI_commander.hpp"
#include "Game_master.hpp"

#include <iostream>
#include <vector>

int main()
{
    std::vector<int> deck1 =
    {
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        HENCHMAN,
        HENCHMAN,
        COMMANDO,
        OPPRESSOR,
        MAIMBOT,
        MAIMBOT,
        GUNKFOOD,
        GUNKFOOD,
        MACHINEPARTS,
        FISSION
    };
    std::vector<int> deck2 =
    {
        BOUNTYHUNTER,
        BOUNTYHUNTER,
        HENCHMAN,
        HENCHMAN,
        COMMANDO,
        OPPRESSOR,
        MAIMBOT,
        MAIMBOT,
        GUNKFOOD,
        GUNKFOOD,
        MACHINEPARTS,
        FISSION
    };

    CLI_commander cmd1, cmd2;

    std::vector<pctrl_ref> commander_refs = {std::ref(cmd1), std::ref(cmd2)};
    std::vector<std::vector<int>> deck_images = {deck1, deck2};

    cmd1.apply_updates();
    Game_master gm(commander_refs, deck_images);
    while(gm.game_loop())
    {
        cmd1.apply_updates();
        cmd2.apply_updates();
    }
    
    return 0;
}