#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include <vector>
#include "UI_element.h"
#include "UI_label.h"
#include "UI_button.h"

class UI_layout : public sf::Drawable, public sf::Transformable
{
    private:
        // list of UI_elements
        std::vector <UI_element*> elements;

        bool isClicked = false;
        int last_clicked_index;

        std::shared_ptr<spdlog::logger> loading_logger;

    public:
        UI_layout();

        // push UI_element into list
        void addElement(UI_element* new_element);
        // mouse hover check
        bool contains(sf::Vector2f curPos);
        bool get_isClicked();
        // pushes hovered element
        void push_click(sf::Vector2f curPos);
        // returns name of the clicked element after mouse button release
        std::string release_click();

        // return number of elements
        int get_elements_size();

        // overriding Drawable methods
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // UI_LAYOUT_H
