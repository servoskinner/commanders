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
        buffer.append(std::string(8, ' ').append("."));  //first divider line

     buffer.append("\n");

     for(int j = 0; j < height; j++)
     {
        //empty spaces
        for(int i = 0; i < 3; i++)
            buffer.append(std::string(width*9, ' ').append(" \n"));  

        //divider
        buffer.append(".");

        for(int i = 0; i < width; i++)
        buffer.append(std::string(8, ' ').append("."));

        buffer.append("\n");
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

     // Render Units
   for(CardInfo card : activeCards)
      if(card.type == Card::UNIT)
      {
         higlightTileBold(buffer, width, height, card.x, card.y);
         // Name
         for(int bias = 1; bias < 9 && bias < card.name.size() + 1; bias++)
            buffer[(card.x*4 + 1)*(width*9 + 2) + card.y*9 + bias] = card.name[bias-1];

         for(int bias = card.name.size() + 1; bias < 9; bias++)
            buffer[(card.x*4 + 1)*(width*9 + 2) + card.y*9 + bias] = '_';
         // Value
         buffer[(card.x*4 + 3)*(width*9 + 2) + card.y*9 + 8] = (std::to_string(card.value % 10))[0];
         buffer[(card.x*4 + 3)*(width*9 + 2) + card.y*9 + 7] = (std::to_string(card.value / 10))[0];
         // Advantage points (if there is any)
         if(card.advantage > 0)
         {
            buffer[(card.x*4 + 3)*(width*9 + 2) + card.y*9 + 2] = (std::to_string(card.advantage % 10))[0];
            buffer[(card.x*4 + 3)*(width*9 + 2) + card.y*9 + 1] = (std::to_string(card.advantage / 10))[0];
         }
         // Overwhelming indicator
         if(card.isOverwhelmed)
            buffer[(card.x*4 + 2)*(width*9 + 2) + card.y*9 + 8] = '!';
         // Ability exhaustion
         if(!card.canMove)
            if(!card.canAttack)
               buffer[(card.x*4 + 2)*(width*9 + 2) + card.y*9 + 1] = 'X';
            else
               buffer[(card.x*4 + 2)*(width*9 + 2) + card.y*9 + 1] = '~';
      }
   std::cout << buffer << std::endl;

   // HAND __________________________________________
   std::cout << "HAND: " << hand.size() \
               << "/?? DECK: " << players[id].deckSize \
               << " DISCARD: " << players[id].discardSize << "\n";
   std::cout << "_______________________________\n";
   std::cout << "YOU HAVE $" << players[id].funds << ":\n";

   for(CardInfo card : hand)
      std::cout << "(" << card.value << ") $" << card.cost << " - " << card.name << ": " << card.text << "\n";
   
   std::cout << "_______________________________\n";
   std::cout << "CONTRACTS: \n";
   // CONTRACTS _____________________________________
   for(CardInfo card : activeCards)
      if(card.type == Card::CONTRACT && card.ownerId == id)
            std::cout << card.name << " (" << card.value << ")\n";

   std::cout << std::endl;

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

      case 'd': case 'D': //Deploy
         action.type = PlayerAction::PLAY;
         std::cout << "SPECIFY CARD NO.:" << std::endl;
         std::cin >> action.args[0];
         if(action.args[0] >= 0 && action.args[0] < hand.size() && hand[action.args[0]].type == Card::UNIT)
         {
            std::cout << "SPECIFY DEPLOYMENT COORDINATES:" << std::endl;
            std::cin >> action.args[1] >> action.args[2];
         }
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
    buffer[x*4*(width*9 + 2) + y*9] = '+';
    buffer[(x+1)*4*(width*9 + 2) + y*9] = '+';
    buffer[x*4*(width*9 + 2) + (y+1)*9] = '+';
    buffer[(x+1)*4*(width*9 + 2) + (y+1)*9] = '+';

    // vertical border
    for(int bias = 1; bias < 4; bias++)
    {
       buffer[(x*4 + bias)*(width*9 + 2) + y*9] = '|';
       buffer[(x*4 + bias)*(width*9 + 2) + (y+1)*9] = '|'; 
    }

    // horizontal border
    for(int bias = 1; bias < 9; bias++)
    {
       buffer[x*4*(width*9 + 2) + y*9 + bias] = '-';
       buffer[(x+1)*4*(width*9 + 2) + y*9 + bias] = '-'; 
    }
}

void TerminalControl::higlightTileLight(std::string &buffer, int width, int height, int x, int y)
{
    // corners
    buffer[x*4*(width*9 + 2) + y*9] = '.';
    buffer[(x+1)*4*(width*9 + 2) + y*9] = ':';
    buffer[x*4*(width*9 + 2) + (y+1)*9] = '.';
    buffer[(x+1)*4*(width*9 + 2) + (y+1)*9] = ':';

    // vertical border
    for(int bias = 1; bias < 4; bias++)
    {
       buffer[(x*4 + bias)*(width*9 + 2) + y*9] = ':';
       buffer[(x*4 + bias)*(width*9 + 2) + (y+1)*9] = ':'; 
    }

    // horizontal border
    for(int bias = 1; bias < 9; bias++)
    {
       buffer[x*4*(width*9 + 2) + y*9 + bias] = '.';
       buffer[(x+1)*4*(width*9 + 2) + y*9 + bias] = '.'; 
    }
}