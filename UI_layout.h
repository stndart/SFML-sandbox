#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include <vector>
#include "UI_element.h"
#include "UI_label.h"
#include "UI_button.h"

class UI_layout : public sf::Drawable, public sf::Transformable
{
    public:
        UI_layout();

        void addElement(UI_element* new_element);
        bool contains(sf::Vector2f curPos);
        bool get_isClicked();
        void push_click(sf::Vector2f curPos);
        std::string release_click();
        //std::string cursor(sf::Vector2f curPos);

        int get_elements_size();

    private:
        std::vector <UI_element*> elements;

        bool isClicked = false;
        int last_clicked_index;
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};

#endif // UI_LAYOUT_H
