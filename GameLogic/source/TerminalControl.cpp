#include <iostream>
#include <string>
#include <vector>

#include "gameLogic.h"
#include "playerController.h"

#define TILE_WIDTH	6
#define TILE_HEIGHT	6

void TerminalControl::printUI()
{
   // GRID _________________________________________
    int width, height;

    height = master->getGridHeight();
    width = master->getGridWidth();

     //draw the empty field
     std::string buffer = ".";

     for(int i = 0; i < width; i++)
        buffer.append(std::string(TILE_WIDTH, ' ').append("."));  //first divider line

     buffer.append("\n");

     for(int j = 0; j < height; j++)
     {
        //empty spaces
        for(int i = 0; i < TILE_HEIGHT; i++)
            buffer.append(std::string(width*(TILE_WIDTH+1), ' ').append(" \n"));  

        //divider
        buffer.append(".");

        for(int i = 0; i < width; i++)
        buffer.append(std::string(TILE_WIDTH, ' ').append("."));

        buffer.append("\n");
     }

     // Highlight deploy zones
     for(int i = 0; i < height; i++)
     {
         higlightTileLight(buffer, width, height, i, 0);
         higlightTileLight(buffer, width, height, i, width - 1);
     }

     // Render Units
   for(CardInfo card : activeCards)
      if(card.type == Card::UNIT)
      {
         if(card.y == 3 || card.y == 4) //in capture zone
            higlightTileFunky(buffer, width, height, card.x, card.y);
         else
            higlightTileBold(buffer, width, height, card.x, card.y);

         // Name
         for(int bias = 1; bias < (TILE_WIDTH+1) && bias < card.name.size() + 1; bias++)
            buffer[(card.x*(TILE_HEIGHT+1) + 1)*(width*(TILE_WIDTH+1) + 2) \
            	   + card.y*(TILE_WIDTH+1) + bias] = card.name[bias-1];

         for(int bias = card.name.size() + 1; bias < (TILE_WIDTH+1); bias++)
            buffer[(card.x*(TILE_HEIGHT+1) + 1)*(width*(TILE_WIDTH+1) + 2) \
            	   + card.y*(TILE_WIDTH+1) + bias] = '_';
         // Value
         buffer[(card.x*(TILE_HEIGHT+1) + TILE_HEIGHT)*(width*(TILE_WIDTH+1) + 2) \
         		+ card.y*(TILE_WIDTH+1) + TILE_WIDTH] = (std::to_string(card.value % 10))[0];
         buffer[(card.x*(TILE_HEIGHT+1) + TILE_HEIGHT)*(width*(TILE_WIDTH+1) + 2) \
         		+ card.y*(TILE_WIDTH+1) + TILE_WIDTH-1] = (std::to_string(card.value / 10))[0];
         // Advantage points (if there is any)
         if(card.advantage > 0)
         {
            buffer[(card.x*(TILE_HEIGHT+1) + TILE_HEIGHT)*(width*(TILE_WIDTH+1) + 2) \
                   + card.y*(TILE_WIDTH+1) + 2] = (std::to_string(card.advantage % 10))[0];
            buffer[(card.x*(TILE_HEIGHT+1) + TILE_HEIGHT)*(width*(TILE_WIDTH+1) + 2) \
            	   + card.y*(TILE_WIDTH+1) + 1] = (std::to_string(card.advantage / 10))[0];
         }
         // Overwhelmed indicator
         if(card.isOverwhelmed)
            buffer[(card.x*(TILE_HEIGHT+1) + 2)*(width*(TILE_WIDTH+1) + 2) \
            	   + card.y*(TILE_WIDTH+1) + TILE_WIDTH] = '!';
         // Ability exhaustion
         if(!card.canMove)
            if(!card.canAttack)
               buffer[(card.x*(TILE_HEIGHT+1) + 2)*(width*(TILE_WIDTH+1) + 2) \
               		  + card.y*(TILE_WIDTH+1) + 1] = 'X';
            else
               buffer[(card.x*(TILE_HEIGHT+1) + 2)*(width*(TILE_WIDTH+1) + 2) \
               		  + card.y*(TILE_WIDTH+1) + 1] = '~';
      }
   std::cout << buffer << std::endl;

   std::cout << "Turn " << master->getTurnAbsolute()+1 << "\n";
   std::cout << "P1: $" << players[0].funds << "       " << players[0].points << " DP       " << players[0].handSize << " in hand\n";
   std::cout << "P2: $" << players[1].funds << "       " << players[1].points << " DP       " << players[1].handSize << " in hand\n";

   // HAND __________________________________________
   
   std::cout << "HAND: " << hand.size() \
               << "/??    DECK: " << players[id].deckSize \
               << "    DISCARD: " << players[id].discardSize << "\n";
   std::cout << "HAND:____________________________\n";
   std::cout << "YOU HAVE $" << players[id].funds << ":\n\n";

   for(int i=0; i<hand.size(); i++)
   {
      std::cout  << "[" << i << "] $" << hand[i].cost << " " << hand[i].name << " (";
      			 (hand[i].value >= 0 ? std::cout << hand[i].value : std::cout << "T") \
      			 << ")" << "\n|   " << hand[i].text << "\n\n";
   }
   
   std::cout << "CONTRACTS:_______________________\n";
   
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

void TerminalControl::handleControllerEvent(int errorCode)
{
   //enum invalidAction {NONE, INVTYPE, NOARGS, INVARGS, PERMISSION, NOSELECT, NOTARGET, EXHAUSTED, NOFUNDS};
   switch (errorCode)
   {
   case Game_master::NONE:
      std::cout << "\nSomething happened...\n" << std::endl;
      break;

   case Game_master::GAME_WIN:
      std::cout << "\nVICTORY! Another successful takeover.\n" << std::endl;
      break;

   case Game_master::GAME_LOSE:
      std::cout << "\nYOU FAILED!\n" << std::endl;
      break;
   
   case Game_master::ACT_INVTYPE:
      std::cout << "\nInvalid command type...\n" << std::endl;
      break;

   case Game_master::ACT_INVARGS:
      std::cout << "\nInvalid argument(s)...\n" << std::endl;
      break;

   case Game_master::ACT_PERMISSION:
      std::cout << "\nYou don't have permission...\n" << std::endl;
      break;

   case Game_master::ACT_NOSELECT:
      std::cout << "\nNo unit has been selected...\n" << std::endl;
      break;

   case Game_master::ACT_NOTARGET:
      std::cout << "\nNo target has been specified...\n" << std::endl;
      break;

   case Game_master::ACT_EXHAUSTED:
      std::cout << "\nOption exhausted...\n" << std::endl;
      break;
   
   case Game_master::ACT_NOFUNDS:
      std::cout << "\nInsufficient funds...\n" << std::endl;
      break;

   case Game_master::UNKNOWN:
      std::cout << "\nAn unknown error occurred...\n" << std::endl;
      break;

   default:
      std::cout << "\nAn even less known error occurred...\n" << std::endl;
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
    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '+';
    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '+';

    // vertical border
    for(int bias = 1; bias < (TILE_HEIGHT+1); bias++)
    {
       buffer[(x*(TILE_HEIGHT+1) + bias)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '|';
       buffer[(x*(TILE_HEIGHT+1) + bias)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '|'; 
    }

    // horizontal border
    for(int bias = 1; bias < (TILE_WIDTH+1); bias++)
    {
       buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '-';
       buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '-'; 
    }
}

void TerminalControl::higlightTileLight(std::string &buffer, int width, int height, int x, int y)
{
	// corners
    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '+';
    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '+';

    // vertical border
    for(int bias = 1; bias < (TILE_HEIGHT+1); bias++)
    {
       buffer[(x*(TILE_HEIGHT+1) + bias)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '.';
       buffer[(x*(TILE_HEIGHT+1) + bias)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '.'; 
    }

    // horizontal border
    // for(int bias = 1; bias < (TILE_WIDTH+1); bias++)
    // {
    //    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '.';
    //    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '.'; 
    // }
}

void TerminalControl::higlightTileFunky(std::string &buffer, int width, int height, int x, int y)
{
	// corners
    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '%';
    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '%';
    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '%';
    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '%';

    // vertical border
    for(int bias = 1; bias < (TILE_HEIGHT+1); bias++)
    {
       buffer[(x*(TILE_HEIGHT+1) + bias)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1)] = '%';
       buffer[(x*(TILE_HEIGHT+1) + bias)*(width*(TILE_WIDTH+1) + 2) + (y+1)*(TILE_WIDTH+1)] = '%'; 
    }

    // horizontal border
    for(int bias = 1; bias < (TILE_WIDTH+1); bias++)
    {
       buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '%';
       buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '%'; 
    }
}
