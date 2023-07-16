#include "UI_layout.h"

UI_layout::UI_layout()
{
    loading_logger = spdlog::get("loading");
}

// push UI_element into list
void UI_layout::addElement(UI_element* new_element)
{
    elements.push_back(new_element);
}

// mouse hover check
bool UI_layout::contains(sf::Vector2f curPos)
{
    bool ans = false;
    for (auto g : elements)
    {
        bool tek = g->contains(curPos);
        //if (g->type_name == "main_menu") {ans = true; break;}
        if (tek) {ans = true; break;}
    }
    return ans;
}

bool UI_layout::get_isClicked()
{
    return isClicked;
}

// pushes hovered element
void UI_layout::push_click(sf::Vector2f curPos)
{
    for (int i = elements.size()-1; i >= 0; i--)
    {
        if (elements[i]->contains(curPos) && elements[i]->isClickable)
        {
            elements[i]->push();
            isClicked = true;
            last_clicked_index = i;
            break;
        }
    }
}

// returns name of the clicked element after mouse button release
std::string UI_layout::release_click()
{
    isClicked = false;
    return elements[last_clicked_index]->release();
}

// return number of elements
int UI_layout::get_elements_size()
{
    return elements.size();
}

void UI_layout::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (auto g : elements)
    {
        target.draw(*g);
    }
}
