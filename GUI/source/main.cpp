#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <vector>
#include <string>

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   450
#define GRID_WIDTH      8
#define GRID_HEIGHT     6
#define BUTTON_WIDTH    55
#define BUTTON_HEIGHT   55
#define GRID_MARGIN_HOR 177
#define GRID_MARGIN_VER 37

inline void resize(sf::Sprite& sprite, int sizeX, int sizeY)
{
    if(sprite.getTexture() == nullptr) //add warning
        return;
    
    float scaleX = float(sizeX) / sprite.getTexture()->getSize().x;
    float scaleY = float(sizeY) / sprite.getTexture()->getSize().y;

    sprite.setScale(scaleX, scaleY);
}

int main() 
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Corporate Wars alpha",
                            sf::Style::Titlebar | sf::Style::Close);

    // Load and scale background image
    sf::Texture backgroundTexture;
    sf::Texture selectorTexture;
    sf::Texture highlightTexture;

    if (!backgroundTexture.loadFromFile("assets/grid.png")) 
    {
        std::cout << "Error loading background texture: assets/grid.png" << std::endl;
        return -1;
    }
    if (!selectorTexture.loadFromFile("assets/legus2.jpeg")) 
    {
        std::cout << "Error loading background texture: assets/grid.png" << std::endl;
        return -1;
    }

    sf::Sprite background(backgroundTexture);
    sf::Sprite selector(selectorTexture);
    
    resize(background, WINDOW_WIDTH, WINDOW_HEIGHT);
    resize(selector, BUTTON_WIDTH, BUTTON_HEIGHT);

    // Create grid
    sf::RectangleShape buttons[GRID_WIDTH][GRID_HEIGHT];

    for (unsigned int i = 0; i < GRID_WIDTH; ++i) {
        for (unsigned int j = 0; j < GRID_HEIGHT; ++j) {
            buttons[i][j].setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
            buttons[i][j].setPosition(GRID_MARGIN_HOR + i * BUTTON_WIDTH,
                                      GRID_MARGIN_VER + j * BUTTON_HEIGHT);
            buttons[i][j].setFillColor(sf::Color::Transparent);
        }
    }

    // Selection coordinates
    int selectedX = -1;
    int selectedY = -1;

    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                    {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (unsigned int i = 0; i < GRID_WIDTH; ++i)
                        for (unsigned int j = 0; j < GRID_HEIGHT; ++j)
                            if (buttons[i][j].getGlobalBounds().contains(mousePos.x, mousePos.y)) 
                            {
                                selectedX = i;
                                selectedY = j;
                                selector.setPosition(GRID_MARGIN_HOR + i * BUTTON_WIDTH,
                                                     GRID_MARGIN_VER + j * BUTTON_HEIGHT);
                            }
                    }
        }
        window.clear();
        window.draw(background);

        for (unsigned int i = 0; i < GRID_WIDTH; ++i)
            for (unsigned int j = 0; j < GRID_HEIGHT; ++j)
                window.draw(buttons[i][j]);

        window.draw(selector);
        window.display();
    }

    return 0;
}