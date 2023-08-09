#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

#include <iostream>
#include <string>

#include "AnimatedSprite.h"

#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class Scene;
class UI_window;

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
    // scale background to fit or crop it
    bool fit_to_background = true;

    // is element in focus (enter focus, when clicked, leave focus, when clicked somewhere else and not on children of element)
    bool focus = false;
    // is element hovered by mouse
    bool hovered = false;
    // time hovered by mouse unmoved
    sf::Time on_hover;
    // time hovered when hint should appear
    sf::Time hover_min = sf::seconds(1.0);
    // cursor position where hovered. Used to stick hint to mouse cursor
    sf::Vector2f hovered_cursor;

    // Scene at which this element is displayed
    Scene* parent_scene = nullptr;
    // Parent UI window (where to create popups, hints, etc)
    UI_window* parent_window = nullptr;

    // pointer to resource manager
    std::shared_ptr<ResourceLoader> resource_manager;

    std::shared_ptr<spdlog::logger> loading_logger, input_logger;

public:
    // is displayed or hidden;
    bool displayed = false;
    // when false, disables hover checks (for hints) for self and children (if UI_window)
    // also, not hoverable element can't be clicked
    bool hoverable = true;

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
    
    // sets displayed for self (and child elements)
    virtual void show(bool displayed = true);

    // mouse hover check
    virtual bool contains(sf::Vector2f cursor) const;
    // set parent window to support hints, popups, nested windows, etc.
    void set_parent_window(UI_window* pwindow);

    // pushes hovered element
    virtual void push_click(sf::Vector2f cursor, bool controls_blocked=false);
    // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
    virtual void release_click(sf::Vector2f cursor, bool controls_bloacked=false, bool skip_action=false);
    
    // if mouse in hovering the element
    virtual bool is_hovered() const;
    // hoveres element under cursor.
    virtual void hover_on(sf::Vector2f cursor);
    // lifts hover under cursor
    virtual void hover_off();
    // sets hoverable to self and children
    virtual void set_hoverable(bool hover=true);

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
    virtual void draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap, int z_shift = 0) const;
};

#endif // UI_ELEMENT_H
