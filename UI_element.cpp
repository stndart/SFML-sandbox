#include "UI_element.h"

UI_element::UI_element()
{
    loading_logger = spdlog::get("loading");

    type_name = "unnamed element";
}

UI_element::UI_element(std::string name, sf::IntRect Input_scale,  sf::Texture* texture_sp) :
    command_output(""), Frame_scale(Input_scale),
    type_name(name), isClickable(false)
{
    loading_logger = spdlog::get("loading");

    sprite = new sf::Sprite(*texture_sp);
    sprite->setPosition(Frame_scale.left, Frame_scale.top);
}

// move left top angle and child sprite to pos
void UI_element::change_position(sf::Vector2f Pos)
{
    Frame_scale.left = Pos.x;
    Frame_scale.top = Pos.y;
    sprite->setPosition(Frame_scale.left, Frame_scale.top);
}

// cursor hover check
bool UI_element::contains(sf::Vector2f curPos)
{
    bool r = Frame_scale.contains((sf::Vector2i)curPos);
    return r;
}

// mouse click action
void UI_element::push() const
{

}

// returns its name when unpushed
std::string UI_element::release() const
{
    return type_name;
}

// draw sprite
void UI_element::draw_element(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (sprite)
    {
        target.draw(*sprite);
    }
}

void UI_element::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    draw_element(target, states);
}
