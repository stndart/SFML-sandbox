#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>

class UI_element : public sf::Drawable, public sf::Transformable
{
    public:
        std::string type_name;

        UI_element();
        UI_element(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp);

        void change_position(sf::Vector2f Pos);
        bool contains(sf::Vector2f curPos);

    protected:
        std::string command_output;
        sf::IntRect Frame_scale;
        sf::Sprite* sprite;
        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // UI_ELEMENT_H
