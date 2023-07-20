#include "UI_button.h"

// label with texture
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Animation* button_spritesheet, bool is_clickable) :
    UI_element(name, UIFrame, button_spritesheet), clickable(is_clickable), pressed(false), callback(NULL), text("")
{
    // None
}

// label with text
UI_button::UI_button(std::string name, sf::IntRect UIFrame, std::string ntext, Animation* button_spritesheet) :
    UI_element(name, UIFrame, button_spritesheet), clickable(false), pressed(false), callback(NULL), text(ntext)
{
    // None
}

// button with callback
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Animation* button_spritesheet, void (*ncallback)()) :
    UI_element(name, UIFrame, button_spritesheet), clickable(true), pressed(false), callback(ncallback), text("")
{
    // None
}

// clickable setter/getter
void UI_button::set_clickable(bool is_clickable)
{
    clickable = is_clickable;
}

bool UI_button::is_clickable() const
{
    return clickable;
}

// callback setter
void UI_button::set_callback(void (*new_callback)())
{
    callback = new_callback;
}

// pushes hovered element
void UI_button::push_click(sf::Vector2f cursor)
{
    if (!contains(cursor))
    {
        set_focus(false);
        return;
    }

    set_focus(true);
    pressed = true;

    // by default: 1 - pressed button, 0 - unpressed
    if (background)
        set_current_frame(1);
}
// releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
void UI_button::release_click(sf::Vector2f cursor, bool skip_action)
{
    pressed = false;
    // by default: 1 - pressed button, 0 - unpressed
    set_current_frame(0);

    if (contains(cursor) && !skip_action && callback)
        callback();
}
