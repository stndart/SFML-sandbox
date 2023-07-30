#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <string>
#include <functional>

#include "UI_element.h"

#include <SFML/System/Time.hpp>

class UI_button : public UI_element
{
    private:
        // flag if callback is invoked, when clicked (and background change also)
        bool clickable;
        // flag if is currently press (to invoke callback, when unpressed)
        bool pressed;
        // if button is immune to controls block (for example, menu button)
        bool ignore_controls_blocking;
        // array of pairs: callback - delay. Call them and see, what happens
        std::vector<std::pair<std::function<void()>, sf::Time> > callbacks;

    public:
        std::string text;

        UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<Animation> button_spritesheet, bool is_clickable = false); // label with texture
        UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::string ntext, std::shared_ptr<Animation> button_spritesheet = NULL); // label with text
        UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<Animation> button_spritesheet, std::function<void()> ncallback); // button with callback

        // clickable setter/getter
        void set_clickable(bool is_clickable);
        bool is_clickable() const;

        // block immune setter/getter
        void set_immune_to_controls_block(bool immune);
        bool get_immune_to_controls_block() const;

        // adding callbacks
        void set_callbacks(std::vector<std::pair<std::function<void()>, sf::Time> > new_callback);
        void add_callback(std::function<void()> callback, sf::Time delay = seconds(0));

        // pushes hovered element
        void push_click(sf::Vector2f cursor, bool controls_blocked=false) override;
        // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
        void release_click(sf::Vector2f cursor, bool controls_blocked=false, bool skip_action=false) override;

        // we don't override draw since it remains the same as in UI_element
};

#endif // UI_BUTTON_H
