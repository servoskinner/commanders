#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "GUI.h"

GUI::GUI() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Corporate Wars alpha",
                            sf::Style::Titlebar | sf::Style::Close)
{
    // Set color constants
    radar_yellow = sf::Color(179, 151, 27);
    radar_green = sf::Color(177, 210, 124);
    radar_lightblue = sf::Color(168, 230, 236);

    // Load textures
    if (!backgroundTexture.loadFromFile("assets/grid.png")) 
        throw std::runtime_error("Error loading texture: assets/grid.png");
    if (!selectorTexture.loadFromFile("assets/legus2.jpeg")) 
        throw std::runtime_error("Error loading texture: assets/legus2.jpeg");

    // Load fonts
    if (!font_futura.loadFromFile("assets/futuraHeavy.ttf")) 
        throw std::runtime_error("Error loading font: assets/futuraHeavy.ttf");

    // Set up textual indicators
    oppHandText.setFont(font_futura);
    oppCreditsText.setFont(font_futura);
    playerCreditsText.setFont(font_futura);
    playerDeckText.setFont(font_futura);
    playerDiscardText.setFont(font_futura);
    leftPointsText.setFont(font_futura);
    rightPointsText.setFont(font_futura);
    
    oppHandText.setCharacterSize(26);
    oppCreditsText.setCharacterSize(26);
    playerCreditsText.setCharacterSize(58);
    playerDeckText.setCharacterSize(24);
    playerDiscardText.setCharacterSize(24);
    leftPointsText.setCharacterSize(18);
    rightPointsText.setCharacterSize(18);

    oppHandText.setFillColor(radar_yellow); 
    oppCreditsText.setFillColor(radar_yellow);
    playerCreditsText.setFillColor(radar_green);
    playerDeckText.setFillColor(radar_lightblue);
    playerDiscardText.setFillColor(radar_lightblue);
    leftPointsText.setFillColor(sf::Color::Black);
    rightPointsText.setFillColor(sf::Color::Black);

    playerCreditsText.setStyle(sf::Text::Bold);
    
    oppHandText.setPosition(36, 26);
    oppCreditsText.setPosition(39, 62);
    playerCreditsText.setPosition(41, 336);
    playerDeckText.setPosition(729, 272);
    playerDiscardText.setPosition(726, 300);
    leftPointsText.setPosition(343, 1);
    rightPointsText.setPosition(429, 1);

    oppHandText.setString("-1");
    oppCreditsText.setString("-1");
    playerCreditsText.setString("0");
    playerDeckText.setString("0");
    playerDiscardText.setString("0");
    leftPointsText.setString("8");
    rightPointsText.setString("8");
    
    // Set up sprites
    background = sf::Sprite(backgroundTexture);
    selector = sf::Sprite(selectorTexture);
    
    resize(background, WINDOW_WIDTH, WINDOW_HEIGHT);
    resize(selector, CARD_WIDTH, CARD_HEIGHT);

    grid = std::vector<std::vector<cardVisualizer>>(GRID_HEIGHT, std::vector<cardVisualizer>(GRID_WIDTH));

    for (unsigned int i = 0; i < GRID_WIDTH; ++i) {
        for (unsigned int j = 0; j < GRID_HEIGHT; ++j) 
        {
            cardVisualizer& tile = grid[i][j];
            tile.enabled = false;

            //resize(tile.sprite, CARD_WIDTH, CARD_HEIGHT);
            tile.name.setCharacterSize(12);
            tile.power.setCharacterSize(16);
            
            tile.name.setFillColor(sf::Color::Black);
            tile.power.setFillColor(sf::Color::Yellow);

            tile.sprite.setPosition(GRID_MARGIN_HOR + i * CARD_WIDTH,
                                      GRID_MARGIN_VER + j * CARD_HEIGHT);
            tile.name.setPosition(GRID_MARGIN_HOR + i * CARD_WIDTH + 2,
                                      GRID_MARGIN_VER + j * CARD_HEIGHT + 2);
            tile.power.setPosition(GRID_MARGIN_HOR + i * CARD_WIDTH + 20,
                                      GRID_MARGIN_VER + j * CARD_HEIGHT + 20);
        }
    }

    // Selection coordinates
    int selectedX = -1;
    int selectedY = -1;

    // while (window.isOpen()) 
    // {
    //     sf::Event event;
        while (window.pollEvent(event)) // Wait until new event occurs
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                    {   // Detect grid button activation
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (unsigned int i = 0; i < GRID_WIDTH; ++i)
                        for (unsigned int j = 0; j < GRID_HEIGHT; ++j)
                            if (grid[i][j].sprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) 
                            {
                                selectedX = i;
                                selectedY = j;

                                oppHandText.setString(std::to_string(selectedX));
                                oppCreditsText.setString(std::to_string(selectedY));

                                selector.setPosition(GRID_MARGIN_HOR + i * CARD_WIDTH,
                                                     GRID_MARGIN_VER + j * CARD_HEIGHT);
                            }
                    }
        }
    }
// }

void GUI::draw()
{
    window.clear();

        window.draw(background);

        window.draw(oppHandText);
        window.draw(oppCreditsText);
        window.draw(playerCreditsText);
        window.draw(playerDeckText);
        window.draw(playerDiscardText);
        window.draw(leftPointsText);
        window.draw(rightPointsText);

        for(auto& line : grid)
            for(cardVisualizer& cv : line)
                if(cv.enabled)
                    cv.draw();

        window.draw(selector);

        window.display();
}