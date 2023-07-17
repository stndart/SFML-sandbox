#include "UI_label.h"

UI_label::UI_label(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp, std::string text_input) : UI_element(name, Input_scale, texture_sp)
{

    isClickable = false;
    /// NOOOO, you can't load fonts in constructors
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        loading_logger->warn("failed to load font - in label");
    }
    sf::Text text;
    text.setFont(font);
    text.setString("Untexted");
    text.setCharacterSize(24);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Magenta);
    text.setPosition(0, 0);
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
