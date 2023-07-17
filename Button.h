#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

// Button is a structure of two sprites, which replace each over when pressed
class Button : public sf::Drawable, public sf::Transformable
{
    private:
        int current_sprite; // 0 or 1
        sf::Sprite* sprite_default;
        sf::Sprite* sprite_pushed;

    public:
        std::string type_name;

        Button(std::string name, sf::Texture* texture_default, sf::Texture* texture_pushed);

        // changes position of both child sprites
        void change_position(sf::Vector2f Pos);
        // checks if sprite contains cursor position
        bool contains(sf::Vector2f curPos);
        // changes sprite cyclically
        void push_button();
        // changes sprite cyclically and returns button name
        std::string release_button();

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // BUTTON_H_INCLUDED
