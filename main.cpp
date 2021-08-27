#include <SFML/Graphics.hpp>
#include <iostream>

#include <conio.h>

#include "AnimatedSprite.h"
#include "VisualEffect.h"

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

    AnimatedSprite animatedSprite("animated", seconds(0.1), true, false);
    Vector2f origin = Vector2f(256.0f / 2, 256.0f / 2);
    animatedSprite.setOrigin(origin);
    animatedSprite.setPosition(Vector2f(200, 200));

    State start = {0, Color(0, 0, 0), Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
    State finish = {0, Color(0, 0, 0), Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
    VisualEffect temp = VisualEffect("temp", &animatedSprite, seconds(3), seconds(80), start, finish);

    State start1 = {0, Color(0, 0, 0), Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
    State finish1 = {0, Color(0, 0, 0), Vector2f(0, 0), Vector2f(200, 0), Vector2f(1, 1)};
    VisualEffect effectedSprite = VisualEffect("effected", &tempí, seconds(4), seconds(1), start1, finish1);

    std::cout << "ani name is " << animatedSprite.name << std::endl;
    std::cout << "temp name is " << temp.name << std::endl;
    std::cout << "eff name is " << effectedSprite.name << std::endl;
    std::cout << std::endl;
    std::cout << "temp child name is " << temp.sprite->name << std::endl;
    std::cout << "eff child name is " << effectedSprite.sprite->name << std::endl;
    std::cout << "but should " << (&temp)->name << std::endl;

    Clock frameClock;


    while (window.isOpen())
    {
        std::cout << "-----------------------\n";

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

                default:
                    break;
                }
            }
        }

        Time frameTime = frameClock.restart();
        effectedSprite.play(*currentAnimation);

        // update AnimatedSprite
        effectedSprite.update(frameTime);

        // draw
        window.clear();
        window.draw(effectedSprite);
        window.display();
    }

    return 0;
}
