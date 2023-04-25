#include <iostream>
#include <string>
#include <vector>

#include "gameLogic.h"
#include "playerController.h"

void TerminalControl::printUI()
{
    int width, height;

    height = master->grid.size();
    width = master->grid[0].size();

     //first, draw the empty field
     std::string buffer = ".";

     for(int i = 0; i < width; i++)
        buffer.append("   .");  //first divider line

     buffer.append(" \n");

     for(int j = 0; j < height; j++)
     {
        //empty spaces
        for(int i = 0; i < 3; i++)
            buffer.append(std::string(width*4, ' ').append(" \n"));  

        //divider
        buffer.append(".");

        for(int i = 0; i < width; i++)
        buffer.append("   .");

        buffer.append(" \n");
     }

     // highlight special regions
     // deploy zones
   //   for(int i = 0; i < height; i++)
   //   {
   //       higlightTileLight(buffer, width, height, i, 0);
   //       higlightTileLight(buffer, width, height, i, width - 1);
   //   }
   //   // combat zones
   //   for(int i = 0; i < height; i++)
   //   {
   //       higlightTileHazard(buffer, width, height, i, width / 2);
   //       higlightTileHazard(buffer, width, height, i, (width - 1) / 2);
   //   }

     // render cards
     for(std::vector<Tile> row : master->grid)
        for(Tile tile : row)
            if(tile.card)
            {
                higlightTileBold(buffer, width, height, tile.x, tile.y);
                // assuming card name is longer than 3 characters
                for(int bias = 0; bias < 3 && bias < tile.card->name.size(); bias++)
                    buffer[(tile.x*4 + 1)*(4*width + 3) + tile.y*4 + 1 + bias - tile.x*3] = tile.card->name[bias];
                //render power values
                buffer[(tile.x*4 + 3)*(4*width + 3) + tile.y*4 + 1 - tile.x*3] = (std::to_string(tile.card->value % 10))[0];
                //render advantage points (if there is any)
                if(tile.card->advantage > 0)
                    buffer[(tile.x*4 + 3)*(4*width + 3) + tile.y*4 - 1 - tile.x*3] = (std::to_string(tile.card->advantage % 10))[0];
            }

     std::cout << buffer << std::endl;
     //render contracts TBA
};

PlayerAction TerminalControl::getAction()
{
   PlayerAction action = {};
   std::string buffer;

while(true)
   {
      std::cout << "PENDING ACTION: (P)ASS - (M)OVE - (A)TTACK" << std::endl;
      std::cin >> buffer;
      
      if(buffer.size() != 1)
      {
         std::cout << "Invalid input;" << std::endl;
         continue;
      }


      switch (buffer[0])
      {
      case 'p': case 'P': //Pass
         action.type = PlayerAction::PASS;
         return action;
         break;

      case 'm': case 'M': //Move
         action.type = PlayerAction::MOVE;
         std::cout << "SPECIFY RECEIVER COORDINATES:" << std::endl;
         std::cin >> action.args[0] >> action.args[1];
         std::cout << "SPECIFY DESTINATION COORDINATES:" << std::endl;
         std::cin >> action.args[2] >> action.args[3];
         return action;
         break;

      case 'a': case 'A': //Attack
         action.type = PlayerAction::ATTACK;
         std::cout << "SPECIFY RECEIVER COORDINATES:" << std::endl;
         std::cin >> action.args[0] >> action.args[1];
         std::cout << "SPECIFY TARGET COORDINATES:" << std::endl;
         std::cin >> action.args[2] >> action.args[3];
         break;
      
      default:
         std::cout << "Invalid input;" << std::endl;
         continue;
      }
   }
}

void TerminalControl::handleActionError(int errorCode)
{
   std::cout << "something happened..." << std::endl;
}

void TerminalControl::applyUpdates()
{
   printUI();
}

void TerminalControl::higlightTileBold(std::string &buffer, int width, int height, int x, int y)
{
    // corners
    buffer[x*4*(4*width + 3) + y*4 - x*3] = '+';
    buffer[x*4*(4*width + 3) + (y+1)*4 - x*3] = '+';
    buffer[(x + 1)*4*(4*width + 3) + y*4 - 3 - x*3] = '+';
    buffer[(x+1)*4*(4*width + 3) + (y + 1)*4 - 3 - x*3] = '+';

    // vertical border
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[(x*4 + (bias + 1))*(4*width + 3)+ y*4  - bias - x*3] = '|';
       buffer[(x*4 + (bias + 1))*(4*width + 3) + (y+1)*4  - bias - x*3] = '|'; 
    }

    // horizontal border
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[x*4*(4*width + 3) + y*4 + (bias + 1) - x*3] = '-';
       buffer[(x + 1)*4*(4*width + 3) + (y+1)*4 + (bias + 1) - 7 - x*3] = '-'; 
    }
}

void TerminalControl::higlightTileLight(std::string &buffer, int width, int height, int x, int y)
{
    // corners
    buffer[x*4*(4*width + 3) + y*4 - x*3] = '.';
    buffer[x*4*(4*width + 3) + (y+1)*4 - x*3] = '.';
    buffer[(x + 1)*4*(4*width + 3) + y*4 - 3 - x*3] = ':';
    buffer[(x+1)*4*(4*width + 3) + (y + 1)*4 - 3 - x*3] = ':';

    // vertical border
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[(x*4 + (bias + 1))*(4*width + 3)+ y*4  - bias - x*3] = ':';
       buffer[(x*4 + (bias + 1))*(4*width + 3) + (y+1)*4  - bias - x*3] = ':'; 
    }

    // horizontal border
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[x*4*(4*width + 3) + y*4 + (bias + 1) - x*3] = '.';
       buffer[(x + 1)*4*(4*width + 3) + (y+1)*4 + (bias + 1) - 7 - x*3] = '.'; 
    }
}

void TerminalControl::higlightTileHazard(std::string &buffer, int width, int height, int x, int y)
{
    // corners
    buffer[x*4*(4*width + 3) + y*4 - x*3] = '7';
    buffer[x*4*(4*width + 3) + (y+1)*4 - x*3] = '7';
    buffer[(x + 1)*4*(4*width + 3) + y*4 - 3 - x*3] = '/';
    buffer[(x+1)*4*(4*width + 3) + (y + 1)*4 - 3 - x*3] = '/';

    // vertical border
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[(x*4 + (bias + 1))*(4*width + 3)+ y*4  - bias - x*3] = '/';
       buffer[(x*4 + (bias + 1))*(4*width + 3) + (y+1)*4  - bias - x*3] = '/'; 
    }

    // horizontal border
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[x*4*(4*width + 3) + y*4 + (bias + 1) - x*3] = '7';
       buffer[(x + 1)*4*(4*width + 3) + (y+1)*4 + (bias + 1) - 7 - x*3] = '7'; 
    }
}