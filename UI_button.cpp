#include "UI_button.h"

UI_button::UI_button()
{
    //ctor
    isClickable = false;
}

UI_button::UI_button(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp, Button* new_button) :
    UI_element(name, Input_scale, texture_sp), button(new_button)
{
    isClickable = true;
}

void UI_button::push()
{
    button->push_button();
}

std::string UI_button::release()
{
    return button->release_button();
}

void UI_button::change_position(sf::Vector2f Pos)
{
    Frame_scale.left = Pos.x;
    Frame_scale.top = Pos.y;
    button->change_position(Pos);
}

void UI_button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    draw_element(target, states);
    if (button)
    {
       target.draw(*button);
    }
}
