#include <iostream>
#include <string>
#include <vector>

#include "gameLogic.h"
#include "playerController.h"

void TerminalControl::printUI()
{
   // GRID _________________________________________
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
   // (it sucks)

     // render cards
     for(std::vector<Tile> row : master->grid)
        for(Tile tile : row)
            if(tile.card)
            {
                higlightTileBold(buffer, width, height, tile.getX(), tile.getY());
                // assuming card name is longer than 3 characters
                for(int bias = 0; bias < 3 && bias < tile.card->name.size(); bias++)
                    buffer[(tile.getX()*4 + 1)*(4*width + 3) + tile.getY()*4 + 1 + bias - tile.getX()*3] = tile.card->name[bias];
                //render power values
                buffer[(tile.getX()*4 + 3)*(4*width + 3) + tile.getY()*4 + 1 - tile.getX()*3] = (std::to_string(tile.card->value % 10))[0];
                //render advantage points (if there is any)
                if(tile.card->advantage > 0)
                    buffer[(tile.getX()*4 + 3)*(4*width + 3) + tile.getY()*4 - 1 - tile.getX()*3] = (std::to_string(tile.card->advantage % 10))[0];
            }

     std::cout << buffer << std::endl;

     // HAND __________________________________________
      std::cout << "HAND: " << master->players[id].hand.size() \
                << "/?? DECK: " << master->players[id].deck.library.size() \
                << "/" << master->players[id].deck.roster.size() \
                << " DISCARD: " << master->players[id].deck.discard.size() << "\n";
      std::cout << "_______________________________\n";
      std::cout << "YOU HAVE $" << master->players[id].funds << ":\n";

      for(Card* cptr : master->players[id].hand)
         std::cout << "(" << cptr->value << ") $" << cptr->cost << "- " << cptr->name << ": " << cptr->text << "\n";
      
      std::cout << "_______________________________\n" << std::endl;
     //render contracts TBA
};

PlayerAction TerminalControl::getAction()
{
   PlayerAction action = {};
   std::string buffer;

while(true)
   {
      std::cout << "PLAYER " << id << " GOES \n"; 
      std::cout << "PENDING ACTION: (P)ASS - (M)OVE - (A)TTACK - (D)EPLOY" << std::endl;
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
         return action;
         break;
      
      default:
         std::cout << "Invalid input;" << std::endl;
         continue;
      }
   }
}

void TerminalControl::handleActionError(int errorCode)
{
   //enum invalidAction {NONE, INVTYPE, NOARGS, INVARGS, PERMISSION, NOSELECT, NOTARGET, EXHAUSTED, NOFUNDS};
   switch (errorCode)
   {
   case GameMaster::NONE:
      std::cout << "something happened..." << std::endl;
      break;
   
   case GameMaster::INVTYPE:
      std::cout << "Invalid command type..." << std::endl;
      break;

   case GameMaster::NOARGS:
      std::cout << "Insufficient arguments..." << std::endl;
      break;

   case GameMaster::INVARGS:
      std::cout << "Invalid argument(s)..." << std::endl;
      break;

   case GameMaster::PERMISSION:
      std::cout << "You don't have permission..." << std::endl;
      break;

   case GameMaster::NOSELECT:
      std::cout << "No unit has been selected..." << std::endl;
      break;

   case GameMaster::NOTARGET:
      std::cout << "No target has been specified..." << std::endl;
      break;

   case GameMaster::EXHAUSTED:
      std::cout << "Option exhausted..." << std::endl;
      break;
   
   case GameMaster::NOFUNDS:
      std::cout << "Insufficient funds..." << std::endl;
      break;

   default:
      std::cout << "Unknown error occurred..." << std::endl;
      break;
   }

}

void TerminalControl::applyUpdates()
{
   if(master->getTurn() == id)
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