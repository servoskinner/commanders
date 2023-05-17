#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <array>

//#include "gameLogic.h"

#define WINDOW_HEADER       "Corporate Wars alpha"

#define WINDOW_WIDTH        800
#define WINDOW_HEIGHT       450
#define GRID_WIDTH          8
#define GRID_HEIGHT         6

#define CARD_WIDTH          55
#define CARD_HEIGHT         55

#define GRID_MARGIN_HOR     177
#define GRID_MARGIN_VER     37

#define PASSBTN_UPPERLEFT_X 640
#define PASSBTN_WIDTH       120
#define PASSBTN_UPPERLEFT_Y 345
#define PASSBTN_HEIGHT      55

#define MENUBTN_UPPERLEFT_X 720
#define MENUBTN_WIDTH       56
#define MENUBTN_UPPERLEFT_Y 26
#define MENUBTN_HEIGHT      54

#define HAND_AREA_WIDTH     400
#define HAND_AREA_Y         380
#define HAND_AREA_X         400
#define HAND_MAX_SPACING    (CARD_WIDTH + 6)

struct cardVisualizer
{
    bool enabled;
    sf::Text name;
    sf::Text power;
    sf::Sprite sprite;
    sf::RectangleShape hitbox;

    void draw(sf::RenderWindow& window)
    {
        window.draw(sprite);
        window.draw(name);
        window.draw(power);
    }
    void setPosition(int x, int y)
    {
        sprite.setPosition(x, y);
        name.setPosition(x, y);
        power.setPosition(x + 39, y + 36);
        hitbox.setPosition(x, y);
    }
};

class GUI
{
    public:
    GUI();

    void draw();
    void clearBoard(); // Set all tile sprites as inactive
    //void renderCard(CardInfo card); // Enable tile visualizer and draw the card;
    void renderCard(int x, int y); // TEMP - renders frog for demonstrational purposes. REPLACE ASAP!!!

    bool pollEvent(); // refreshes the "event" variable. Returns true if event queue was non-empty.
    std::array<int, 2> checkGridHit(); // Checks whether a tile was pressed. Return {-1, -1} if not.
    bool checkPassHit(); // Check whether the pass button was pressed.
    int checkCardHit(); // Check whether a card was pressed.
    bool checkMenuHit(); // Check if the menu button was pressed.
    bool isActive() { return window.isOpen();}

    void clear();
    void pushHand();
    void updateCompactPositions(); // Update everything that is placed in a compact manner, such as contracts and player hand

    // Info for indicator rendering
    int opponentHandSize;
    int opponentFunds;
    int playerFunds;
    int playerDeckSize;
    int playerDiscardSize;
    int leftPoints;
    int rightPoints;
    void updateText();

    protected:
    sf::RenderWindow window;
    sf::Event event;
    // Textures
    sf::Texture backgroundTexture;
    sf::Texture selectorTexture; // temp
    sf::Texture highlightTexture; // temp

    sf::Font font_futura;

    sf::Color radar_yellow;
    sf::Color radar_green;
    sf::Color radar_lightblue;

    std::vector<sf::Texture> cardTextures; // card ids correspond to indices

    sf::Sprite background;
    //sf::Sprite selector;
    sf::RectangleShape passHitbox;
    sf::RectangleShape menuHitbox;
    // Grid visualizer
    std::vector<std::vector<cardVisualizer>> grid;
    std::vector<cardVisualizer> hand;
    std::vector<cardVisualizer> contractsLeft;
    std::vector<cardVisualizer> contractsRight;
    // Indicator texts
    sf::Text oppHandText; //players[1 - id].handSize
    sf::Text oppCreditsText; //players[1 - id].funds
    sf::Text playerCreditsText; //players[id].funds
    sf::Text playerDeckText; //players[id].deckSize
    sf::Text playerDiscardText; //players[id].discardSize
    sf::Text leftPointsText; //players[0].points
    sf::Text rightPointsText; //players[1].points

    inline void resize(sf::Sprite& sprite, int sizeX, int sizeY) // Resize a sprite by setting new dimensions in pixels.
    {
        if(sprite.getTexture() == nullptr) //add warning
            return;
        
        float scaleX = float(sizeX) / sprite.getTexture()->getSize().x;
        float scaleY = float(sizeY) / sprite.getTexture()->getSize().y;

        sprite.setScale(scaleX, scaleY);
    }
};