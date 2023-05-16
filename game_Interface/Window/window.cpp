#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>


void main_menu(sf::RenderWindow & window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
        window.close();
    }

    sf::Texture texture_start, texture_about, texture_exit, texture_menu_background, texture_about_bg;;
    texture_start.loadFromFile("Assets/images/main_menu_images/start_game.png");
    texture_about.loadFromFile("Assets/images/main_menu_images/about.png");
    texture_exit.loadFromFile("Assets/images/main_menu_images/exit.png");
    texture_menu_background.loadFromFile("Assets/images/main_menu_images/background.png");
    texture_about_bg.loadFromFile("Assets/images/main_menu_images/about_authors_1.png");


    sf::Sprite tx_menu_start(texture_start), tx_menu_about(texture_about), tx_menu_exit(texture_exit), 
        tx_menu_bg(texture_menu_background), tx_about_bg(texture_about_bg);
    

    int isMenu = 1; // нужно ли сейчас рисовать меню или нет
    int menu_num = 0; // элемент меню
    tx_menu_start.setPosition(100, 30);
    tx_menu_about.setPosition(100, 90);
    tx_menu_exit.setPosition(100, 150);
    tx_menu_bg.setPosition(825, 70);
    tx_about_bg.setPosition(0, 0);


    //tx_menu_start.scale(sf::Vector2f(1.2, 1.2));
    //tx_menu_about.scale(sf::Vector2f(1.2, 1.2));
    //tx_menu_exit.scale(sf::Vector2f(1.2, 1.2));
    tx_menu_bg.scale(sf::Vector2f(1.2, 1.2));
    
    
    //// menu ////

    while (isMenu) {

        tx_menu_start.setColor(sf::Color::White);
        tx_menu_about.setColor(sf::Color::White);
        tx_menu_exit.setColor(sf::Color::White);
        menu_num = 0;
        window.clear(sf::Color(183, 71, 96));

        if (sf::IntRect(100, 30, 300, 50).contains(sf::Mouse::getPosition(window))) { tx_menu_start.setColor(sf::Color::Yellow); menu_num = 1; }
        if (sf::IntRect(100, 90, 300, 50).contains(sf::Mouse::getPosition(window))) { tx_menu_about.setColor(sf::Color::Yellow); menu_num = 2; }
        if (sf::IntRect(100, 150, 300, 50).contains(sf::Mouse::getPosition(window))) { tx_menu_exit.setColor(sf::Color::Yellow); menu_num = 3; }
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) { window.close(); isMenu = false; }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (menu_num == 1) isMenu = false; // меню закрывается - игра начинается
            // может здесь вызывать функцию game_menu?
            // 
            if (menu_num == 2) {
                while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                    
                    window.draw(tx_about_bg);
                    window.display();
                    //window.clear(sf::Color(183, 71, 96));
                }
            }

            if (menu_num == 3) { window.close(); isMenu = false; }
        }

        window.draw(tx_menu_start);
        window.draw(tx_menu_about);
        window.draw(tx_menu_exit);
        window.draw(tx_menu_bg);
        

        window.display();
    }
    //////////////////
}


void game_menu(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
        window.close();
    }
    int isGame = 1; // нужно ли сейчас рисовать игровое поле или нет
    // нужна смена очков

    // нажатие на кнопку меню
    sf::Texture clicked_three_dots_button;
    clicked_three_dots_button.loadFromFile("Assets/images/game_process_images/pressed_button_1.png");
    sf::Sprite tx_clicked_button(clicked_three_dots_button);

    while (isGame) {
        if (sf::IntRect(1737, 0, 183, 156).contains(sf::Mouse::getPosition(window))) { tx_clicked_button.setColor(sf::Color::Yellow); }
    }
}

int main() {
    sf::Event event;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Corporate Wars", sf::Style::Default);

    //////////// SETTING ICON ///////////////////
    
    //do not use icon12.png
    sf::Image icon; // create image object
    if (!icon.loadFromFile("Assets/images/icon.png")) {
        return 1;
    }
    window.setIcon(976, 976, icon.getPixelsPtr());
    ///////////////////////////////////////////////


    /////////// SOUND //////////
    /*
    sf::Music soundtrack;
    if (!soundtrack.openFromFile("Assets/sounds/cw_sound.ogg")) {
        return -1;
    }
    soundtrack.play();
     
    if (soundtrack.getStatus() != sf::Music::Status::Playing) {
        soundtrack.play();
    }
    */
    /////////////////////////////
        

    //main_menu(window); // вызов меню

    window.setFramerateLimit(60);

    //creating time
    //sf::Clock clock();


    ////////////// CREATING GAME BACKGROUND ////////////////////
    sf::Image game_background_image;
    game_background_image.loadFromFile("Assets/images/game_process_images/game_background_image_final.png");
    sf::Texture game_background;
    game_background.loadFromImage(game_background_image);
    sf::Sprite s_game_background;
    s_game_background.setTexture(game_background);
    ////////////////////////////////////////////////////////////


    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // подсветка правой верхней кнопки в игровом меню
            // добавить внешнее условие, что координата в пределах для тех трёх кнопок (это подсвечивает кнопку, вывод картинки)
            
            /*
            if (event.type == sf::Event::MouseButtonPressed) { // когда нажимается мышь
                if (event.key.code == sf::Mouse::Left) { // левая кнопка мыши
                    // настройки игры (выйти, помощь, продолжить, добавить крестик?)
                }
            }
            */
        }

        window.clear();
        window.draw(s_game_background);
        window.display();
    }

    return 0;
}