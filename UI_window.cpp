#include "UI_window.h"

UI_window::UI_window(std::string name, sf::IntRect UIFrame, bool is_framed) : UI_element(name, UIFrame),
isFramed(is_framed), pressed(false), clicked_child(NULL)
{
    displayed = true;
}

// adds UI_element into set with z_index
void UI_window::addElement(UI_element* new_element, int z_index)
{
    displayed = true;

    elements.insert(std::make_pair(z_index, new_element));
}

// mouse hover check
bool UI_window::contains(sf::Vector2f cursor)
{
    bool res = UI_element::contains(cursor);

    for (auto pi : elements)
        if (pi.second->contains(cursor))
            res = true;

//    input_logger->trace("Window {} at {}x{} contains? {}", name, cursor.x, cursor.y, res);

    return res;
}

bool UI_window::is_clicked() const
{
    return pressed;
}

// pushes hovered element
void UI_window::push_click(sf::Vector2f cursor)
{
    input_logger->trace("Window {} clicked at {}x{}", name, cursor.x, cursor.y);

    // if cursor missed, then don't click, and release focus
    if (!contains(cursor))
    {
        set_focus(false);

        for (auto pi : elements)
            pi.second->set_focus(false);

        return;
    }

    pressed = true;

    for (auto pi : elements)
    {
        UI_element* uie = pi.second;
        if (uie->contains(cursor))
        {
            uie->push_click(cursor);
            clicked_child = uie;
            break;
        }
    }
}

// releases push (and invokes callback if hovered element is pushed)
void UI_window::release_click(sf::Vector2f cursor, bool skip_action)
{
    input_logger->trace("Window {} released from {}x{}", name, cursor.x, cursor.y);

    pressed = false;
    // transfer event to pushed child. If <skip_action>, no callback is called regardless of <cursor>
    if (clicked_child)
    {
        clicked_child->release_click(cursor, skip_action);
        clicked_child = NULL;
    }
}

// return number of elements
std::size_t UI_window::get_elements_size() const
{
    return elements.size();
}

// overriding Drawable methods
void UI_window::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    UI_element::draw(target, states);

    for (auto g : elements)
    {
        target.draw(*g.second);
    }
}
