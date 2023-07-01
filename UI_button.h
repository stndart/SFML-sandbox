#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "UI_element.h"
#include "Button.h"

class UI_button : public UI_element
{
    private:
        Button* button;

    public:
        UI_button();
        UI_button(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp, Button* new_button);

        // changes button state
        virtual void push() const override;
        // returns button name and changes button state
        virtual std::string release() const override;

        // moves UI element as well as button
        void change_position(sf::Vector2f Pos);

        // overriding Drawable methods
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};

#endif // UI_BUTTON_H
