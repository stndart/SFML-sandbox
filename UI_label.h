#ifndef UI_LABEL_H
#define UI_LABEL_H

#include <iostream>
#include "UI_element.h"

class UI_label : public UI_element
{
    private:
        sf::Text text;
    public:
        UI_label();
        UI_label(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp, std::string text_input);
        void set_text(std::string input_text);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // UI_LABEL_H
