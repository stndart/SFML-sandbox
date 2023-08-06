#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <iostream>
#include <string>

#include "AnimatedSprite.h"

#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class Scene;

class UI_element : public sf::Drawable, public sf::Transformable
{
protected:
    // sprite position and rectangle to hover checks
    sf::IntRect Frame_scale;

    // spritesheet with textures
    std::shared_ptr<Animation> background_animation;
    // index of current frame in the spritesheet
    int cur_frame;
    // sprite to draw textures
    sf::Sprite background;

    // is element in focus (enter focus, when clicked, leave focus, when clicked somewhere else and not on children of element)
    bool focus = false;
    // is element hovered by mouse
    bool hovered = false;
    // time hovered by mouse unmoved
    sf::Time on_hover;

    // Scene at which this element is displayed
    Scene* parent_scene;
    // to control hierarchical window structure
    sf::Vector2f parent_coords;

    // pointer to resource manager
    std::shared_ptr<ResourceLoader> resource_manager;

    std::shared_ptr<spdlog::logger> loading_logger, input_logger;

public:
    // is displayed or hidden;
    bool displayed = false;

    std::string name;
    // z-index, at which it is displayed in <parent_scene>
    int z_index;

    UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload);
    UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Texture> background);
    UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Animation> spritesheet);

    // Frame_scale setter/getter
    virtual void setFrame(sf::IntRect new_frame_scale);
    sf::IntRect getFrame() const;
    // Animation setter
    void setAnimation(std::shared_ptr<Animation> spritesheet);
    // current frame setter/getter
    void set_current_frame(int new_frame);
    std::shared_ptr<sf::Texture> getTexture() const;
    const sf::IntRect& getTextureFrame() const;
    // focus setter/getter
    void set_focus(bool new_focus);
    bool is_focused() const;
    // if mouse in hovering the element
    bool is_hovered() const;

    // mouse hover check
    bool contains(sf::Vector2f cursor) const;
    // set parent coords to support nested windows
    void set_parent_coords(sf::Vector2f pcoords);

    // pushes hovered element
    virtual void push_click(sf::Vector2f cursor, bool controls_blocked=false);
    // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
    virtual void release_click(sf::Vector2f cursor, bool controls_bloacked=false, bool skip_action=false);

    // overriding some Sprite methods
    void setColor(const Color& color);

    // overriding Transformable methods
    virtual void move(const Vector2f &offset);
    virtual void scale(const Vector2f &factor);
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;
    virtual void setPosition(const Vector2f &position);
    virtual void setPosition(float x, float y);
    virtual void setOrigin(const Vector2f &origin);
    virtual void setOrigin(float x, float y);
    virtual void setScale(const Vector2f &factors);
    virtual void setScale(float factorX, float factorY);

    // handling mouse movements
    virtual void update(Event& event);
    virtual void update(sf::Time deltatime);

    // overriding Drawable methods
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    // before drawing send itself to sort by z-index
    virtual void draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap) const;
};

#endif // UI_ELEMENT_H
