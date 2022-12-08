#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "UI_element.h"
#include "Button.h"

class UI_button : UI_element
{
    public:
        UI_button();
        UI_button(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp, Button* new_button);

        void push();
        std::string release();

        void change_position(sf::Vector2f Pos);

    private:
        Button* button;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // UI_BUTTON_H
