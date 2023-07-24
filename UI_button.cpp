#include "UI_button.h"
#include "Scene.h"

// label with texture
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, Animation* button_spritesheet, bool is_clickable) : UI_element(name, UIFrame, parent)
{
    loading_logger->trace("UI_button:UI_button {} #1, UIFRame +{}+{}, {}x{}", name,
                          UIFrame.left, UIFrame.top, UIFrame.width, UIFrame.height);
    displayed = true;

    setAnimation(button_spritesheet);

    clickable = is_clickable;
    pressed = false;
    text = "";
}

// label with text
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::string ntext, Animation* button_spritesheet) :
    UI_element(name, UIFrame, parent, button_spritesheet), clickable(false), pressed(false), text(ntext)
{
    displayed = true;

    loading_logger->trace("UI_button:UI_button {} #2, UIFRame +{}+{}, {}x{}", name,
                          UIFrame.left, UIFrame.top, UIFrame.width, UIFrame.height);
}

// button with callback
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, Animation* button_spritesheet, std::function<void()> ncallback) :
    UI_element(name, UIFrame, parent, button_spritesheet), clickable(true), pressed(false), text("")
{
    displayed = true;

    if (ncallback)
        add_callback(ncallback);

    loading_logger->trace("UI_button:UI_button {} #3, UIFRame +{}+{}, {}x{}", name,
                          UIFrame.left, UIFrame.top, UIFrame.width, UIFrame.height);
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

// adding callbacks
void UI_button::set_callbacks(std::vector<std::pair<std::function<void()>, sf::Time> > new_callbacks)
{
    callbacks.clear();
    callbacks = new_callbacks;
}

void UI_button::add_callback(std::function<void()> callback, sf::Time delay)
{
    if (!callback)
    {
        loading_logger->error("Adding empty callback");
        throw;
    }
    callbacks.push_back(std::make_pair(callback, delay));
}

// pushes hovered element
void UI_button::push_click(sf::Vector2f cursor)
{
    input_logger->trace("Button {} clicked at {}x{}", name, cursor.x, cursor.y);

    // becomes not focused, when clicked anywhere else
    if (!contains(cursor))
    {
        set_focus(false);
        return;
    }

    // becomes focused when clicked
    set_focus(true);
    pressed = true;

    // button logic: changes texture, when <pressed> is changed
    // by default: 1 - pressed button, 0 - unpressed
    if (background && clickable)
        set_current_frame(1);
}

// releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
void UI_button::release_click(sf::Vector2f cursor, bool skip_action)
{
    input_logger->trace("Button {} released from {}x{}", name, cursor.x, cursor.y);

    pressed = false;

    // button logic
    if (clickable)
    {
        // changes texture, when <pressed> is changed
        // by default: 1 - pressed button, 0 - unpressed
        if (background)
            set_current_frame(0);

        // if released due to focus change, then don't invoke callback
        if (contains(cursor) && !skip_action && callbacks.size() > 0)
        {
            input_logger->debug("Button {} calling callback", name);
            for (std::pair<std::function<void()>, sf::Time> callpair : callbacks)
            {
                parent_scene->add_callback(callpair.first, callpair.second);
            }
        }
    }
}
