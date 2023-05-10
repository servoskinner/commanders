#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Simple Program");

    sf::Texture texture1, texture2;
    if (!texture1.loadFromFile("../assets/image1.png") || !texture2.loadFromFile("../assets/image2.png")) {
        std::cout << "Error loading textures" << std::endl;
        return -1;
    }

    sf::Sprite background1(texture1);
    sf::Sprite background2(texture2);

    sf::RectangleShape button1(sf::Vector2f(100, 50));
    button1.setFillColor(sf::Color::Green);
    button1.setPosition(350, 500);

    sf::RectangleShape button2(sf::Vector2f(100, 50));
    button2.setFillColor(sf::Color::Blue);
    button2.setPosition(350, 500);

    bool screen1 = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (screen1 && button1.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        screen1 = false;
                    } else if (!screen1 && button2.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        screen1 = true;
                    }
                }
            }
        }

        window.clear();
        if (screen1) {
            window.draw(background1);
            window.draw(button1);
        } else {
            window.draw(background2);
            window.draw(button2);
        }
        window.display();
    }

    return 0;
}