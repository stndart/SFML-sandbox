#include <SFML/Graphics.hpp>
#include <iostream>

#include <conio.h>

#include "Cell.h"
#include "AnimatedSprite.h"

using namespace sf;
using namespace std;

int KeyMotion;

void Motion()
{
    switch (KeyMotion)
    {
    case 1:
        cout << "up" << endl;
        break;

    case 2:
        cout << "down" << endl;
        break;

    case 3:
        cout << "left" << endl;
        break;

    case 4:
        cout << "right" << endl;
        break;
    }
}

int main()
{
    int key;
    // setup window
    Vector2i screenDimensions(800,600);
    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation");
    window.setFramerateLimit(60);

    // load texture
    Texture texture;
    if (!texture.loadFromFile("Images/tex.png"))
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

    Animation* currentAnimation = &loading;

    AnimatedSprite animatedSprite(seconds(0.2), true, false);
    animatedSprite.setPosition(Vector2f(screenDimensions / 4));

    Clock frameClock;


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
            if (event.type == Event::KeyPressed){
                switch (event.key.code)
                {
                case Keyboard::W:
                    KeyMotion = 1;
                    Motion();
                    break;

                case Keyboard::S:
                    KeyMotion = 2;
                    Motion();
                    break;

                case Keyboard::A:
                    KeyMotion = 3;
                    Motion();
                    break;

                case Keyboard::D:
                    KeyMotion = 4;
                    Motion();
                    break;
                }
            }
        }

        Time frameTime = frameClock.restart();
        animatedSprite.play(*currentAnimation);

        // update AnimatedSprite
        animatedSprite.update(frameTime);

        // draw
        window.clear();
        window.draw(animatedSprite);
        window.display();
    }

    return 0;
}
