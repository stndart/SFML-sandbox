#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <iostream>
#include "spdlog/spdlog.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>

class UI_element : public sf::Drawable, public sf::Transformable
{
    protected:
        std::string command_output;
        // sprite position and rectangle to hover checks
        sf::IntRect Frame_scale;
        sf::Sprite* sprite;

        // draw sprite
        void draw_element(sf::RenderTarget& target, sf::RenderStates states) const;

        std::shared_ptr<spdlog::logger> loading_logger;

    public:
        std::string type_name;
        bool isClickable;

        /// need destructor
        UI_element();
        UI_element(std::string name, sf::IntRect Input_scale, sf::Texture* texture_sp);

        // move left top angle and child sprite to pos
        void change_position(sf::Vector2f Pos);
        // cursor hover check
        bool contains(sf::Vector2f curPos);

        // mouse push action
        virtual void push() const;
        // returns its name when unpushed
        virtual std::string release() const;

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    private:
};

#endif // UI_ELEMENT_H
