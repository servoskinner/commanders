#include <iostream>
#include <string>
#include <vector>

#include "gameLogic.h"

void higlightTileBold(std::string &buffer, int width, int height, int x, int y);

void printUI(const GameMaster& gm)
{
    int width, height;

    height = gm.grid.size();
    width = gm.grid[0].size();

     //first, draw the empty field
     std::string buffer = ".";

     for(int i = 0; i < width; i++)
        buffer.append("   .");  //first divider line

     buffer.append("\n");

     for(int j = 0; j < height; j++)
     {
        //empty spaces
        for(int i = 0; i < 3; i++)
            buffer.append(std::string(width*4, ' ').append("\n"));  

        //divider
        buffer.append(".");

        for(int i = 0; i < width; i++)
        buffer.append("   .");

        buffer.append("\n");
     }

     // render cards
     for(std::vector<Tile> row : gm.grid)
        for(Tile tile : row)
            if(tile.card)
            {
                higlightTileBold(buffer, width, height, tile.x, tile.y);
                // assuming card name is longer than 3 characters
                for(int bias = 0; bias < 3 && bias < tile.card->name.size(); bias++)
                    buffer[(tile.y*4 + 1)*(4*width + 2) + tile.x*4 + 1 + bias - tile.y*3] = tile.card->name[bias];

                buffer[(tile.y*4 + 3)*(4*width + 2) + tile.x*4 + 1 - tile.y*3] = (std::to_string(tile.card->value % 10))[0];
            }

     std::cout << buffer << std::endl;
     //render contracts
};

void higlightTileBold(std::string &buffer, int width, int height, int x, int y)
{
    // + as corners
    buffer[y*4*(4*width + 2) + x*4 - y*3] = '+';
    buffer[y*4*(4*width + 2) + (x+1)*4 - y*3] = '+';
    buffer[(y + 1)*4*(4*width + 2) + x*4 - 3 - y*3] = '+';
    buffer[(y+1)*4*(4*width + 2) + (x + 1)*4 - 3 - y*3] = '+';

    // vertical |
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[(y*4 + (bias + 1))*(4*width + 2)+ x*4  - bias - y*3] = '|';
       buffer[(y*4 + (bias + 1))*(4*width + 2) + (x+1)*4  - bias - y*3] = '|'; 
    }

    // horizontal -
    for(int bias = 0; bias < 3; bias++)
    {
       buffer[y*4*(4*width + 2) + x*4 + (bias + 1) - y*3] = '-';
       buffer[(y + 1)*4*(4*width + 2) + (x+1)*4 + (bias + 1) - 7 - y*3] = '-'; 
    }
}