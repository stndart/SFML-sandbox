#ifndef BUTTON_H_INCLUDED
#define BUTTON_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class Button : public sf::Drawable, public sf::Transformable
{
    public:
        std::string type_name;

        Button();
        Button(std::string name, sf::Texture* texture_default, sf::Texture* texture_pushed);

        void change_position(sf::Vector2f Pos);
        bool contains(sf::Vector2f curPos);
        void push_button();
        std::string release_button();

    private:
        int current_sprite; // 0 or 1
        sf::Sprite* sprite_default;
        sf::Sprite* sprite_pushed;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // BUTTON_H_INCLUDED
