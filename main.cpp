#include <SFML/Graphics.hpp>
#include <iostream>

#include "Cell.h"

using namespace sf;
using namespace std;

int main()
{
    int a;
    RenderWindow window (VideoMode(500, 500), "GBI");

    Image Qwar2;
    Qwar2.loadFromFile("Images/Quar.png");

    Texture Qwar0;
    Qwar0.loadFromImage(Qwar2);

    Sprite Qwar;
    Qwar.setTexture(Qwar0);
    Qwar.setPosition(0, 0);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed){
                window.close();
            }
        }

        window.clear();
        window.draw(Qwar);
        window.display();
    }
    return 0;
}
