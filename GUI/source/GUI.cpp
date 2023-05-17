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

    opponentHandSize = 0;
    opponentFunds = 0;
    playerFunds = 0;
    playerDeckSize = 0;
    playerDiscardSize = 0;
    leftPoints = 0;
    rightPoints = 0;

    updateText();
    
    // Set up sprites
    background = sf::Sprite(backgroundTexture);
    //selector = sf::Sprite(selectorTexture);
    
    resize(background, WINDOW_WIDTH, WINDOW_HEIGHT);
    //resize(selector, CARD_WIDTH, CARD_HEIGHT);

    passHitbox.setPosition(PASSBTN_UPPERLEFT_X, PASSBTN_UPPERLEFT_Y);
    passHitbox.setSize({PASSBTN_WIDTH, PASSBTN_HEIGHT});
    //passHitbox.setFillColor(sf::Color::Red);

    menuHitbox.setPosition(MENUBTN_UPPERLEFT_X, MENUBTN_UPPERLEFT_Y);
    menuHitbox.setSize({MENUBTN_WIDTH, MENUBTN_HEIGHT});
    //menuHitbox.setFillColor(sf::Color::Red);

    grid = std::vector<std::vector<cardVisualizer>>(GRID_HEIGHT, std::vector<cardVisualizer>(GRID_WIDTH));

    for (unsigned int i = 0; i < GRID_HEIGHT; i++)
        for (unsigned int j = 0; j < GRID_WIDTH; j++) 
        {
            cardVisualizer& tile = grid[i][j];
            tile.enabled = false;

            tile.hitbox.setSize({CARD_WIDTH, CARD_HEIGHT});

            tile.name.setFont(font_futura);
            tile.power.setFont(font_futura);

            tile.name.setCharacterSize(12);
            tile.power.setCharacterSize(16);
            
            tile.name.setFillColor(sf::Color::Black);
            tile.power.setFillColor(sf::Color::Black);

            tile.setPosition(GRID_MARGIN_HOR + j * CARD_WIDTH, GRID_MARGIN_VER + i * CARD_HEIGHT);
        }
}

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

        //window.draw(passHitbox);
        //window.draw(menuHitbox);

        //add hand/contract rendering
        for(cardVisualizer& cv : hand)
            cv.draw(window);

        for(auto& line : grid)
            for(cardVisualizer& cv : line)
                if(cv.enabled)
                    cv.draw(window);

        //window.draw(selector);

        window.display();
}

void GUI::clear() // Set all tile sprites as inactive
{
    hand = {};
    contractsLeft = {};
    contractsRight = {};

    for(auto& line : grid)
            for(cardVisualizer& cv : line)
                cv.enabled = false;
}

//     //void renderCard(CardInfo card); // Enable tile visualizer and draw the card;
void GUI::renderCard(int x, int y) // TEMP - renders frog for demonstrational purposes. REPLACE ASAP!!!
{
    std::cout << "loaded" << std::endl;

    cardVisualizer& tile = grid[x][y];
    tile.enabled = true;
    
    tile.sprite.setTexture(selectorTexture);
    resize(tile.sprite, CARD_WIDTH, CARD_HEIGHT);
    
    tile.name.setString("frog");
    tile.power.setString("2");
}

bool GUI::pollEvent() // refreshes the "event" variable. Returns true if event queue was non-empty.
{
    bool ret = window.pollEvent(event);
    if(ret && event.type == sf::Event::Closed)
    {
        window.close();
        return false;
    }
    return ret;
}

std::array<int, 2> GUI::checkGridHit() // Checks whether a tile was pressed. Return {-1, -1} if not.
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {   // Detect grid button activation
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for (unsigned int i = 0; i < GRID_HEIGHT; ++i)
            for (unsigned int j = 0; j < GRID_WIDTH; ++j)
                if (grid[i][j].hitbox.getGlobalBounds().contains(mousePos.x, mousePos.y)) 
                    return {i, j};
    }
    return {-1, -1};
}

bool GUI::checkPassHit() // Check whether the pass button was pressed.
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return passHitbox.getGlobalBounds().contains(mousePos.x, mousePos.y);
    }
    return false;
}

bool GUI::checkMenuHit() // Check whether menu button was pressed.
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        return menuHitbox.getGlobalBounds().contains(mousePos.x, mousePos.y);
    }
    return false;
}


int GUI::checkCardHit()
{
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        for(int i = 0; i < hand.size(); i++)
            if(hand[i].hitbox.getGlobalBounds().contains(mousePos.x, mousePos.y))
                return i;
    }
    return -1;
}
 // Check whether a card was pressed.
//     bool checkMenuHit(); // Check if the menu button was pressed.

void GUI::updateCompactPositions()
{
    int spacing = std::min(static_cast<int>(HAND_AREA_WIDTH / hand.size()), HAND_MAX_SPACING);

    for(int i = 0; i < hand.size(); i++)
        hand[i].setPosition(HAND_AREA_X - spacing*hand.size()/2 + i*spacing, 
                            HAND_AREA_Y);
}

void GUI::updateText()
{
    oppHandText.setString(std::to_string(opponentHandSize));
    oppCreditsText.setString(std::to_string(opponentFunds));
    playerCreditsText.setString(std::to_string(playerFunds));
    playerDeckText.setString(std::to_string(playerDeckSize));
    playerDiscardText.setString(std::to_string(playerDiscardSize));
    leftPointsText.setString(std::to_string(leftPoints));
    rightPointsText.setString(std::to_string(rightPoints));
}

void GUI::pushHand()
{
    cardVisualizer cv;

    cv.enabled = true;

    cv.hitbox.setSize({CARD_WIDTH, CARD_HEIGHT});

    cv.name.setFont(font_futura);
    cv.power.setFont(font_futura);

    cv.name.setCharacterSize(12);
    cv.power.setCharacterSize(16);
            
    cv.name.setFillColor(sf::Color::Black);
    cv.power.setFillColor(sf::Color::Black);
    
    cv.sprite.setTexture(selectorTexture);
    resize(cv.sprite, CARD_WIDTH, CARD_HEIGHT);
    
    cv.name.setString("frog");
    cv.power.setString("2");

    hand.push_back(cv);
    std::cout << hand.size() << std::endl;
}