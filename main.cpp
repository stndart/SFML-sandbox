#include <SFML/Graphics.hpp>
#include <iostream>

#include <conio.h>

#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "Scene.h"

using namespace sf;
using namespace std;

int KeyMotion;

void Motion()
{
    switch (KeyMotion)
    {
    case 1:
        cout << "click" << endl;
        break;
    }
}

int main()
{
    // setup window
    Vector2i screenDimensions(1280, 720);
    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation");
    window.setFramerateLimit(60);

    // load textures

    Texture menu_texture;
    if (!menu_texture.loadFromFile("Images/menu.jpg"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture new_button_texture;
    if (!new_button_texture.loadFromFile("Images/new_game_button.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture new_button_pushed_texture;
    if (!new_button_pushed_texture.loadFromFile("Images/new_game_button_pushed.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Clock frameClock;

    Scene main_menu = new_menu_scene(&menu_texture, &new_button_texture, &new_button_pushed_texture, screenDimensions);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed){
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape){
                window.close();
            }
            main_menu.update(event);
        }

        Time frameTime = frameClock.restart();

        main_menu.update(frameTime);

        // draw
        window.clear();
        window.draw(main_menu);
        window.display();
    }

    return 0;
}
