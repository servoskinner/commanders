#include "gameLogic.h"

#include <iostream>

int printUI(const GameMaster& gm);
void higlightTileBold(std::string &buffer, int width, int height, int x, int y);

int main()
{
    GameMaster master = GameMaster();

    Card sample_card = Card();
    Card enemy = Card();
    Card toughEnemy = Card();
    Card enemy_adv = Card();

    sample_card.name = "YOU";
    sample_card.value = 5;

    enemy.name = "BADGUY";
    enemy.value = 2;

    enemy_adv.name = "TANK";
    enemy_adv.value = 3;
    enemy_adv.advantage = 1;

    sample_card.Deploy(&(master.grid[5][5]));
    enemy.Deploy(&(master.grid[3][4]));
    enemy_adv.Deploy(&(master.grid[1][3]));
    
    int direction;
    bool returned = false;
    bool attacked = false;

    while(true)
    {
        printUI(master);

        std::cout << "7-0-4\n" << "|   |\n" << "3 + 1\n" << "|   |\n" << "6-2-5\n";

        if(sample_card.value <= 0)
        {
            std::cout << "\nYou've died.\n" << std::endl;
            break;
        }

        std::cout << "\nX:" << sample_card.gridPosition->x << " Y:" << sample_card.gridPosition->y << std::endl;

        try { std::cin >> direction; }
        catch(const std::exception& excpt) { std::cout << "Invalid input" << std::endl; }

        if(direction > 7 || direction < 0)
            std::cout << "Invalid input" << std::endl;
        else if(direction < 4)
        {
            std::cout << "Moving out..." << std::endl;
            returned = sample_card.Move(direction);
            if(!returned)
            {
                std::cout << "Something is blocking the way..." << std::endl;
                returned = sample_card.Attack(direction);
                if(!returned)
                    std::cout << "It does not seem to make any sense..." << std::endl;
                else
                    std::cout << "Engaging combat..." << std::endl;
            }
        }
        else
        {
            returned = sample_card.Attack(direction);
            if(!returned)
                    std::cout << "There is nothing to attack..." << std::endl;
                else
                    std::cout << "Engaging combat..." << std::endl;
        }
    }

    return 0;
}