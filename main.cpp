#include <SFML/Graphics.hpp>
#include <iostream>

#include <conio.h>

#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "Scene.h"
#include "Field.h"

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

    Texture player_texture;
    if (!player_texture.loadFromFile("Images/player.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture field_bg_texture;
    if (!field_bg_texture.loadFromFile("Images/field_bg.jpg"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture grass_block_texture;
    if (!grass_block_texture.loadFromFile("Images/grass.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Clock frameClock;

    Scene main_menu = new_menu_scene(&menu_texture, &new_button_texture, &new_button_pushed_texture, screenDimensions);
    //Scene game_scene = new_game_scen(&game_texture)
    Field game_field = new_field_scene(&field_bg_texture, 100, 100, &grass_block_texture, &player_texture, screenDimensions);
    //cout << "field maked\n";

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
            //main_menu.update(event);
            game_field.update(event);
        }

        Time frameTime = frameClock.restart();

        //main_menu.update(frameTime);
        game_field.update(frameTime);

        // draw
        window.clear();
        //cout << "window has been cleared" << endl;
        window.draw(game_field);
        //window.draw(main_menu);
        //cout << "window has been drawn" << endl;
        window.display();
        //return 0;
    }

    return 0;
}
