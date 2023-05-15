#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>


void menu(sf::RenderWindow & window) {
    sf::Texture texture_start, texture_about, texture_exit;
    texture_start.loadFromFile("Assets/images/main_menu_images/start_game.png");
    texture_about.loadFromFile("Assets/images/main_menu_images/about.png");
    texture_exit.loadFromFile("Assets/images/main_menu_images/exit.png");


    sf::Sprite tx_menu_start(texture_start), tx_menu_about(texture_about), tx_menu_exit(texture_exit);
    

    bool isMenu = 1; // нужно ли сейчас рисовать меню или нет
    int menu_num = 0; // элемент меню
    tx_menu_start.setPosition(100, 30);
    tx_menu_about.setPosition(100, 90);
    tx_menu_exit.setPosition(100, 150);

    //// menu ////

    while (isMenu) {

    }
}







int main()
{
    sf::Event event;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Corporate Wars", sf::Style::Fullscreen);

    window.setFramerateLimit(60);

    //creating time
    //sf::Clock clock();


    //creating game background
    sf::Image game_background_image;
    game_background_image.loadFromFile("Assets/images/game_background_image_final.png");
    sf::Texture game_background;
    game_background.loadFromImage(game_background_image);
    sf::Sprite s_game_background;
    s_game_background.setTexture(game_background);


    //setting icon
    //do not use icon12.png
    sf::Image icon; // create image object
    if (!icon.loadFromFile("Assets/images/icon.png")) {
        return 1;
    }
    window.setIcon(976, 976, icon.getPixelsPtr());



    while (window.isOpen())
    {

        sf::Vector2i pixelPos = sf::Mouse::getPosition(window); //координаты курсора


        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // подсветка правой верхней кнопки в игровом меню
            // добавить внешнее условие, что координата в пределах дл€ тех трЄх кнопок (это подсвечивает кнопку, вывод картинки)
            if (event.type == sf::Event::MouseButtonPressed) { // когда нажимаетс€ мышь
                if (event.key.code == sf::Mouse::Left) { // лева€ кнопка мыши
                    // настройки игры (выйти, помощь, продолжить, добавить крестик?)
                }
            }
        }

        window.clear();
        window.draw(s_game_background);
        window.display();
    }

    return 0;
}