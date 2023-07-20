#include "UI_window.h"

UI_window::UI_window(std::string name, sf::IntRect UIFrame, bool is_framed) : UI_element(name, UIFrame), isFramed(is_framed)
{
    // None
}

// adds UI_element into set with z_index
void UI_window::addElement(UI_element* new_element, int z_index)
{
    elements.insert(std::make_pair(z_index, new_element));
}

// mouse hover check
bool UI_window::contains(sf::Vector2f cursor)
{
    for (auto g : elements)
        if (g.second->contains(cursor))
            return true;

    return UI_element::contains(cursor);
}

bool UI_window::is_clicked() const
{
    return isClicked;
}

// pushes hovered element
void UI_window::push_click(sf::Vector2f cursor)
{
    // if cursor missed, then don't click, and release focus
    if (!contains(cursor))
    {
        set_focus(false);

        for (auto pi : elements)
            pi.second->set_focus(false);

        return;
    }

    isClicked = true;

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
    isClicked = false;
    // transfer event to pushed child. If <skip_action>, no callback is called regardless of <cursor>
    clicked_child->release_click(cursor, skip_action);
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
