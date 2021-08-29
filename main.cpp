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
<<<<<<< HEAD
    }

    Texture new_button_pushed_texture;
    if (!new_button_pushed_texture.loadFromFile("Images/new_game_button_pushed.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
=======
    }

    Texture new_button_pushed_texture;
    if (!new_button_pushed_texture.loadFromFile("Images/new_game_button_pushed.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Animation loading;
    loading.setSpriteSheet(texture);
    loading.addFrame(IntRect(0, 0, 256, 256));
    loading.addFrame(IntRect(256, 0, 256, 256));
    loading.addFrame(IntRect(512, 0, 256, 256));
    loading.addFrame(IntRect(768, 0, 256, 256));
    loading.addFrame(IntRect(0, 256, 256, 256));
    loading.addFrame(IntRect(256, 256, 256, 256));
    loading.addFrame(IntRect(512, 256, 256, 256));
    loading.addFrame(IntRect(768, 256, 256, 256));

    //Animation* currentAnimation = &loading;

    AnimatedSprite animatedSprite("animated", seconds(0.1), true, false);
    Vector2f origin = Vector2f(256.0f / 2, 256.0f / 2);
    animatedSprite.setOrigin(origin);
    animatedSprite.setPosition(Vector2f(200, 200));

    State start = {0, Color(255, 255, 255), Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
    State finish = {360 * 80, Color(255, 255, 255), Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
    VisualEffect a1 = VisualEffect("temp", &animatedSprite, seconds(1), seconds(80), start, finish);

    State start1 = {0, Color(255, 255, 255), Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
    State finish1 = {0, Color(0, 255, 0), Vector2f(400, 0), Vector2f(0, 0), Vector2f(1, 1)};
    VisualEffect a2 = VisualEffect("effected", &a1, seconds(2), seconds(2), start1, finish1);
    VisualEffect effectedSprite = VisualEffect("effected", &a2, seconds(5), seconds(2), finish1, start1);
>>>>>>> c167c65bd18d0815f20432e6594df7a741aaccfa

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
<<<<<<< HEAD
            }
=======
            }
>>>>>>> c167c65bd18d0815f20432e6594df7a741aaccfa
            main_menu.update(event);
        }

        // draw
        window.clear();
        window.draw(main_menu);
        window.display();
    }

    return 0;
}
