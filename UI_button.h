#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <string>
#include <functional>

#include "UI_element.h"

class UI_button : public UI_element
{
    private:
        // flag if callback is invoked, when clicked (and background change also)
        bool clickable;
        // flag if is currently press (to invoke callback, when unpressed)
        bool pressed;
        // callback. Call it and see, what happens
        std::function<void()> callback;

    public:
        std::string text;

        UI_button(std::string name, sf::IntRect UIFrame, Animation* button_spritesheet, bool is_clickable = false); // label with texture
        UI_button(std::string name, sf::IntRect UIFrame, std::string ntext, Animation* button_spritesheet = NULL); // label with text
        UI_button(std::string name, sf::IntRect UIFrame, Animation* button_spritesheet, std::function<void()> ncallback); // button with callback

        // clickable setter/getter
        void set_clickable(bool is_clickable);
        bool is_clickable() const;
        // callback setter
        void set_callback(std::function<void()> new_callback);

        // pushes hovered element
        void push_click(sf::Vector2f cursor) override;
        // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
        void release_click(sf::Vector2f cursor, bool skip_action=false) override;

        // we don't override draw since it remains the same as in UI_element
};

#endif // UI_BUTTON_H
