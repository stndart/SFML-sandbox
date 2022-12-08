#include "UI_element.h"

UI_element::UI_element()
{
    type_name = "unnamed element";
}

UI_element::UI_element(std::string name, sf::IntRect Input_scale,  sf::Texture* texture_sp)
 : type_name(name), Frame_scale(Input_scale)
{
    sf::Sprite* sprite_0 = new sf::Sprite(*texture_sp);
    sprite = sprite_0;
}

void UI_element::change_position(sf::Vector2f Pos)
{
    Frame_scale.left = Pos.x;
    Frame_scale.top = Pos.y;
}

bool UI_element::contains(sf::Vector2f curPos)
{
    bool r = Frame_scale.contains((sf::Vector2i)curPos);
    return r;
}

void UI_element::push()
{
    //for init
}

std::string UI_element::release()
{
    //for init
    return "???";
}

void UI_element::draw_element(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (sprite)
    {
        //std::cout << type_name << std::endl;
        sprite->setPosition(Frame_scale.left, Frame_scale.top);
        target.draw(*sprite);
    }
}

void UI_element::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    draw_element(target, states);
}
