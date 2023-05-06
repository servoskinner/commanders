#pragma once
#include <SFML/Graphics.hpp>



int main()
{
    sf::Event event;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Corporate Wars", sf::Style::Fullscreen);

    //creating time
    //sf::Clock clock();


    //creating game background
    sf::Image game_background_image;
    game_background_image.loadFromFile("Source/images/game_background_image.jpg");
    sf::Texture game_background;
    game_background.loadFromImage(game_background_image);
    sf::Sprite s_game_background;
    s_game_background.setTexture(game_background);


    while (window.isOpen())
    {

        //setting icon
        //do not use icon12.png
        sf::Image icon; // create image object
        if (!icon.loadFromFile("Source/images/icon.png")) {
            return 1;
        }
        window.setIcon(976, 976, icon.getPixelsPtr());


        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(s_game_background);
        window.display();
    }

    return 0;
}