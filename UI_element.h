#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <iostream>
#include <string>

#include "AnimatedSprite.h"

#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class UI_element : public sf::Drawable, public sf::Transformable
{
    protected:
        // sprite position and rectangle to hover checks
        sf::IntRect Frame_scale;
        // spritesheet with textures
        Animation* background;
        // coordinates of frame on spritesheet
        Vertex m_vertices[4];
        // index of current frame in the spritesheet
        int cur_frame;

        // is element in focus (enter focus, when clicked, leave focus, when clicked somewhere else and not on children of element)
        bool focus;

        // Scene at which this element is displayed
        Scene* parent_scene;

        std::shared_ptr<spdlog::logger> loading_logger, input_logger;

    public:
        std::string name;
        // is displayed or hidden;
        bool displayed;
        // z-index, at which it is displayed in <parent_scene>
        int z_index;

        UI_element(std::string name, sf::IntRect UIFrame, Scene* parent);
        UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, Animation* spritesheet);

        // Frame_scale setter/getter
        void setFrame(sf::IntRect new_frame_scale);
        sf::IntRect getFrame() const;
        // Animation setter
        void setAnimation(Animation* spritesheet);
        // current frame setter/getter
        void set_current_frame(int new_frame);
        sf::Texture* getTexture() const;
        const sf::IntRect& getTextureFrame() const;
        // focus setter/getter
        void set_focus(bool new_focus);
        bool is_focused() const;

        // mouse hover check
        bool contains(sf::Vector2f cursor) const;

        // pushes hovered element
        virtual void push_click(sf::Vector2f cursor);
        // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
        virtual void release_click(sf::Vector2f cursor, bool skip_action=false);

        // overriding Transformable methods
        virtual void move(const Vector2f &offset);
        FloatRect getLocalBounds() const;
        FloatRect getGlobalBounds() const;
        virtual void setPosition(const Vector2f &position);

        // overriding Drawable methods
        virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // UI_ELEMENT_H
