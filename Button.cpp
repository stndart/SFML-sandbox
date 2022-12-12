#include "Button.h"

Button::Button()
{
    //NONE
}

/*Button::Button(std::name = "unnamed") type_name(name)
{

}*/

Button::Button(std::string name, sf::Texture* texture_default, sf::Texture* texture_pushed) : type_name(name)
{
    ///////////////////////////////////////////////////
    sf::Sprite* sprite1 = new sf::Sprite(*texture_default);
//    sf::Vector2u t_d_size = texture_default->getSize();
//    float t_d_x = t_d_size.x / 2;
//    float t_d_y = t_d_size.y / 2;
//    sprite1->setOrigin(sf::Vector2f{t_d_x, t_d_y});
    ///////////////////////////////////////////////////
    sf::Sprite* sprite2 = new sf::Sprite(*texture_pushed);
//    sf::Vector2u t_p_size = texture_pushed->getSize();
//    float t_p_x = t_p_size.x / 2;
//    float t_p_y = t_p_size.y / 2;
//    sprite2->setOrigin(sf::Vector2f{t_p_x, t_p_y});
    ///////////////////////////////////////////////////
    sprite_default = sprite1;
    sprite_pushed = sprite2;

    current_sprite = 0; //default
}

void Button::change_position(sf::Vector2f Pos)
{
    sprite_default->setPosition(Pos);
    sprite_pushed->setPosition(Pos);
}

bool Button::contains(sf::Vector2f curPos)
{
    if ((sprite_default->getGlobalBounds()).contains(curPos))
    {
        return true;
    }
    return false;
}

void Button::push_button()
{
    current_sprite += 1;
    current_sprite %= 2;
}

std::string Button::release_button() // может проверять тут на курсор?
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
