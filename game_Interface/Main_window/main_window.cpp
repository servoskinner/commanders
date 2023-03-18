#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

/*
int main_funk()
{
    sf::RenderWindow window(sf::VideoMode(640, 480), "Corporate Wars v.0.0.1", sf::Style::Titlebar | sf::Style::Close);
    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            }

        }
        window.clear(); // clear old frame
        window.display(); // tell app that window is done drawing
    }
    return 0;
}
*/