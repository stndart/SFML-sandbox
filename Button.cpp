#include "Button.h"

Button::Button()
{
    //NONE
}

Button::Button(std::string name, sf::Texture* texture_default, sf::Texture* texture_pushed) : type_name(name)
{
    sprite_default = new sf::Sprite(*texture_default);
    sprite_pushed = new sf::Sprite(*texture_pushed);

    current_sprite = 0; //default
}

// changes position of both child sprites
void Button::change_position(sf::Vector2f Pos)
{
    sprite_default->setPosition(Pos);
    sprite_pushed->setPosition(Pos);
}

// checks if sprite contains cursor position
bool Button::contains(sf::Vector2f curPos)
{
    if ((sprite_default->getGlobalBounds()).contains(curPos))
    {
        return true;
    }
    return false;
}

// changes sprite cyclically
void Button::push_button()
{
    current_sprite += 1;
    current_sprite %= 2;
}

// changes sprite cyclically and returns button name
std::string Button::release_button()
{
    current_sprite += 1;
    current_sprite %= 2;
    return type_name;
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (current_sprite == 0)
    {
        target.draw(*sprite_default);
    }
    else
    {
        target.draw(*sprite_pushed);
    }
}
