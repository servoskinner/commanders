#include "gameLogic.h"

#include <stdexcept>
#include <iostream>

GameMaster::GameMaster(std::vector<ControllerPtr> controllers, std::vector<DeckPtr> decks, int gridHeight = 6, int gridWidth = 8)
{
    if(controllers.size() > decks.size()) 
        throw std::runtime_error("Insufficient decks specified for game creation;");
    if(controllers.size() < decks.size())
        std::clog << "Redundant decks provided for GameMaster constructor. Ignoring last;" << std::endl;

    players = {};
    playerControllers = controllers;

    grid = std::vector<std::vector<Tile>>(gridHeight, std::vector<Tile>(gridWidth, Tile(*this)));

    for(int row = 0; row < gridHeight; row++)
        for(int column = 0; column < gridWidth; column ++)
            grid[row][column].y = column, grid[row][column].x = row; 

    for(int playerid = 0; playerid < controllers.size(); playerid++)
        players.push_back(Player(*decks[playerid], playerid))

    // deployZones = {}; //TBA
    // captureZone = {};
}