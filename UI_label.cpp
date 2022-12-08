#include "UI_label.h"

UI_label::UI_label() : UI_element()
{
    isClickable = false;
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "failed to load font - in label\n";
    }
    sf::Text text;
    text.setFont(font);
    text.setString("Untexted");
    text.setCharacterSize(24);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Magenta);
    text.setPosition(0, 0);
}

UI_label::UI_label(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp, std::string text_input) : UI_label()
{
    UI_element(name, Input_scale, texture_sp);
}

void UI_label::set_text(std::string input_text)
{
    text.setString(input_text);
}

void UI_label::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    draw_element(target, states);
    target.draw(text);
}
