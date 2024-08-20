#include <iostream>
#include <string>
#include <vector>

#include "CLI_commander.hpp"

#define TILE_WIDTH_CHARS	6
#define TILE_HEIGHT_CHARS	6

void CLI_commander::render_UI()
{
   // GRID _________________________________________
     //draw the empty field
     std::string buffer = ".";

     for(int i = 0; i < game_params.grid_size.first; i++)
        buffer.append(std::string(TILE_WIDTH_CHARS, ' ').append("."));  //first divider line

     buffer.append("\n");

     for(int j = 0; j < game_params.grid_size.second; j++)
     {
        // empty spaces
        for(int i = 0; i < TILE_HEIGHT_CHARS; i++)
            buffer.append(std::string(game_params.grid_size.first*(TILE_WIDTH_CHARS+1), ' ').append(" \n"));  

        // divider
        buffer.append(".");

        for(int i = 0; i < game_params.grid_size.first; i++)
        buffer.append(std::string(TILE_WIDTH_CHARS, ' ').append("."));

        buffer.append("\n");
     }

     // Highlight deploy zones
     for(int i = 0; i < game_params.grid_size.second; i++)
     {
         highlight_tile_subtle(buffer, game_params.grid_size.first, i, 0);
         highlight_tile_subtle(buffer, game_params.grid_size.first, i, game_params.grid_size.first - 1);
     }

     // Render Units
   for(Card_info card : game_status.active_cards)
   {
      Description_generator::Card_descr original = Description_generator::get_card_instance(card.card_id);
      if(card.type == CTYPE_UNIT)
      {
         if(card.y == 3 || card.y == 4) //in capture zone
            highlight_tile_funky(buffer, game_params.grid_size.first, card.x, card.y);
         else
            highlight_tile_bold(buffer, game_params.grid_size.first, card.x, card.y);

         // Name
         for(int bias = 1; bias < (TILE_WIDTH_CHARS+1) && bias < original.name.size() + 1; bias++)
            buffer[(card.x*(TILE_HEIGHT_CHARS+1) + 1)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
            	   + card.y*(TILE_WIDTH_CHARS+1) + bias] = original.name[bias-1];

         for(int bias = original.name.size() + 1; bias < (TILE_WIDTH_CHARS+1); bias++)
            buffer[(card.x*(TILE_HEIGHT_CHARS+1) + 1)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
            	   + card.y*(TILE_WIDTH_CHARS+1) + bias] = '_';
         // Value
         buffer[(card.x*(TILE_HEIGHT_CHARS+1) + TILE_HEIGHT_CHARS)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
         		+ card.y*(TILE_WIDTH_CHARS+1) + TILE_WIDTH_CHARS] = (std::to_string(card.value % 10))[0];
         buffer[(card.x*(TILE_HEIGHT_CHARS+1) + TILE_HEIGHT_CHARS)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
         		+ card.y*(TILE_WIDTH_CHARS+1) + TILE_WIDTH_CHARS-1] = (std::to_string(card.value / 10))[0];
         // Advantage points (if there is any)
         if(card.advantage > 0)
         {
            buffer[(card.x*(TILE_HEIGHT_CHARS+1) + TILE_HEIGHT_CHARS)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
                   + card.y*(TILE_WIDTH_CHARS+1) + 2] = (std::to_string(card.advantage % 10))[0];
            buffer[(card.x*(TILE_HEIGHT_CHARS+1) + TILE_HEIGHT_CHARS)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
            	   + card.y*(TILE_WIDTH_CHARS+1) + 1] = (std::to_string(card.advantage / 10))[0];
         }
         // Overwhelmed indicator
         if(card.is_overwhelmed)
            buffer[(card.x*(TILE_HEIGHT_CHARS+1) + 2)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
            	   + card.y*(TILE_WIDTH_CHARS+1) + TILE_WIDTH_CHARS] = '!';
         // Ability exhaustion
         if(!card.can_move)
            if(!card.can_attack)
               buffer[(card.x*(TILE_HEIGHT_CHARS+1) + 2)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
               		  + card.y*(TILE_WIDTH_CHARS+1) + 1] = 'X';
            else
               buffer[(card.x*(TILE_HEIGHT_CHARS+1) + 2)*(game_params.grid_size.first*(TILE_WIDTH_CHARS+1) + 2) \
               		  + card.y*(TILE_WIDTH_CHARS+1) + 1] = '~';
      }
   }
   std::cout << buffer << std::endl;

   std::cout << "Turn " << game_status.turn_absolute + 1 << "\n";
   std::cout << "P1: $" << game_status.players[0].funds << "       " << game_status.players[0].points << " DP       " << game_status.players[0].hand_size << " in hand\n";
   std::cout << "P2: $" << game_status.players[1].funds << "       " << game_status.players[1].points << " DP       " << game_status.players[1].hand_size << " in hand\n";

   // HAND __________________________________________
   
   std::cout << "HAND: " << game_status.hands[active_id].size() \
               << "/??    DECK: " << game_status.players[active_id].library_size \
               << "    DISCARD: " << game_status.players[active_id].discard_size << "\n";
   std::cout << "HAND:____________________________\n";
   std::cout << "YOU HAVE $" << game_status.players[active_id].funds << ":\n\n";

   for(int i=0; i<game_status.hands[active_id].size(); i++)
   {
      Description_generator::Card_descr original = Description_generator::get_card_instance(game_status.hands[active_id][i].card_id);

      std::cout  << "[" << i << "] $" << game_status.hands[active_id][i].cost << " " << original.name << " (";
      			 (game_status.hands[active_id][i].value >= 0 ? std::cout << game_status.hands[active_id][i].value : std::cout << "T") \
      			 << ")" << "\n|   " << original.ability_text << "\n\n";
   }
   
   std::cout << "CONTRACTS:_______________________\n";
   
   // CONTRACTS _____________________________________
   for(Card_info card : game_status.active_cards)
   {
      if(card.type == CTYPE_CONTRACT && card.owner_id == active_id)
      {
         Description_generator::Card_descr origin = Description_generator::get_card_instance(card.entity_id);
         std::cout << origin.name << " (" << card.value << ")\n";
      }
   }
   std::cout << std::endl;

};

Commander::Order CLI_commander::get_order()
{
   Order order = {};
   std::string buffer;

while(true)
   {
      render_UI();

      std::cout << "PLAYER " << active_id << " GOES \n"; 
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
         order.type = Order::ORD_PASS;
         return order;
         break;

      case 'm': case 'M': //Move
         order.data.resize(4);
         order.type = Order::ORD_MOVE;
         std::cout << "SPECIFY RECEIVER COORDINATES:" << std::endl;
         std::cin >> order.data[0] >> order.data[1];
         std::cout << "SPECIFY DESTINATION COORDINATES:" << std::endl;
         std::cin >> order.data[2] >> order.data[3];
         return order;
         break;

      case 'a': case 'A': //Attack
         order.data.resize(4);
         order.type = Order::ORD_ATTACK;
         std::cout << "SPECIFY RECEIVER COORDINATES:" << std::endl;
         std::cin >> order.data[0] >> order.data[1];
         std::cout << "SPECIFY TARGET COORDINATES:" << std::endl;
         std::cin >> order.data[2] >> order.data[3];
         return order;
         break;

      case 'd': case 'D': //Deploy
         order.data.resize(1);
         order.type = Order::ORD_PLAY_CARD;
         std::cout << "SPECIFY CARD NO.:" << std::endl;
         std::cin >> order.data[0];
         if(order.data[0] >= 0 && order.data[0] < game_status.hands[active_id].size() && game_status.hands[active_id][order.data[0]].type == CTYPE_UNIT)
         {
            order.data.resize(3);
            std::cout << "SPECIFY DEPLOYMENT COORDINATES:" << std::endl;
            std::cin >> order.data[1] >> order.data[2];
         }
         return order;
         break;
      
      
      default:
         std::cout << "Invalid input;" << std::endl;
         continue;
      }
   }
}

void CLI_commander::process_order_feedback(int code)
{
   //enum invalidAction {INVORD_NONE, INVTYPE, NOARGS, INVARGS, PERMISSION, NOSELECT, NOTARGET, EXHAUSTED, NOFUNDS};
   switch(code)
   {
      case Order::ORD_SUCCESS:
         std::cout << "\nExecuted\n" << std::endl;
         break;

      case Order::ORD_INVTYPE:
         std::cout << "\nInvalid command type...\n" << std::endl;
         break;

      case Order::ORD_INVARGS:
         std::cout << "\nInvalid argument(s)...\n" << std::endl;
         break;

      case Order::ORD_PERMISSION:
         std::cout << "\nYou don't have permission...\n" << std::endl;
         break;

      case Order::ORD_NOSELECT:
         std::cout << "\nNo unit has been selected...\n" << std::endl;
         break;

      case Order::ORD_NOTARGET:
         std::cout << "\nNo target has been specified...\n" << std::endl;
         break;

      case Order::ORD_EXHAUSTED:
         std::cout << "\nOption exhausted...\n" << std::endl;
         break;
      
      case Order::ORD_NOFUNDS:
         std::cout << "\nInsufficient funds...\n" << std::endl;
         break;

      case Order::ORD_UNKNOWN:
         std::cout << "\nAn unknown error occurred...\n" << std::endl;
         break;

      default:
         std::cout << "\nAn even less known error occurred...\n" << std::endl;
         break;
   }
}

void CLI_commander::apply_updates()
{
   if(game_status.turn == active_id)
      render_UI();
}

void CLI_commander::highlight_tile_bold(std::string &buffer, int g_width, int x, int y)
{
    // corners
    buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '+';
    buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '+';

    // vertical border
    for(int bias = 1; bias < (TILE_HEIGHT_CHARS+1); bias++)
    {
       buffer[(x*(TILE_HEIGHT_CHARS+1) + bias)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '|';
       buffer[(x*(TILE_HEIGHT_CHARS+1) + bias)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '|'; 
    }

    // horizontal border
    for(int bias = 1; bias < (TILE_WIDTH_CHARS+1); bias++)
    {
       buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1) + bias] = '-';
       buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1) + bias] = '-'; 
    }
}

void CLI_commander::highlight_tile_subtle(std::string &buffer, int g_width, int x, int y)
{
	// corners
    buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '+';
    buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '+';
    buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '+';

    // vertical border
    for(int bias = 1; bias < (TILE_HEIGHT_CHARS+1); bias++)
    {
       buffer[(x*(TILE_HEIGHT_CHARS+1) + bias)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '.';
       buffer[(x*(TILE_HEIGHT_CHARS+1) + bias)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '.'; 
    }

    // horizontal border
    // for(int bias = 1; bias < (TILE_WIDTH+1); bias++)
    // {
    //    buffer[x*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '.';
    //    buffer[(x+1)*(TILE_HEIGHT+1)*(width*(TILE_WIDTH+1) + 2) + y*(TILE_WIDTH+1) + bias] = '.'; 
    // }
}

void CLI_commander::highlight_tile_funky(std::string &buffer, int g_width, int x, int y)
{
	// corners
    buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '%';
    buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '%';
    buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '%';
    buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '%';

    // vertical border
    for(int bias = 1; bias < (TILE_HEIGHT_CHARS+1); bias++)
    {
       buffer[(x*(TILE_HEIGHT_CHARS+1) + bias)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1)] = '%';
       buffer[(x*(TILE_HEIGHT_CHARS+1) + bias)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + (y+1)*(TILE_WIDTH_CHARS+1)] = '%'; 
    }

    // horizontal border
    for(int bias = 1; bias < (TILE_WIDTH_CHARS+1); bias++)
    {
       buffer[x*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1) + bias] = '%';
       buffer[(x+1)*(TILE_HEIGHT_CHARS+1)*(g_width*(TILE_WIDTH_CHARS+1) + 2) + y*(TILE_WIDTH_CHARS+1) + bias] = '%'; 
    }
}

/*

CONTROLS TODO
tab - toggle hand
shift - see additional game data
wasd - move cursor 

rclick deployed cards - see descriptions

scroll card descriptions

*/