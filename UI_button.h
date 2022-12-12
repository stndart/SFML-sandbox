#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "UI_element.h"
#include "Button.h"

class UI_button : public UI_element
{
    public:
        UI_button();
        UI_button(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp, Button* new_button);

        virtual void push() const override;
        virtual std::string release() const override;

        void change_position(sf::Vector2f Pos);

    private:
        Button* button;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // UI_BUTTON_H
