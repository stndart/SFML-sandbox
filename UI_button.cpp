#include "UI_button.h"

#include "Scene.h"
#include "ResourceLoader.h"

// label with texture
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Texture> button_texture) :
    UI_element(name, UIFrame, parent, resload, button_texture)
{
    loading_logger->trace("UI_button:UI_texture {}, UIFRame +{}+{}, {}x{}", name,
                          UIFrame.left, UIFrame.top, UIFrame.width, UIFrame.height);
    displayed = true;
}

// label with texture
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Animation> button_spritesheet, bool is_clickable) :
    UI_element(name, UIFrame, parent, resload)
{
    loading_logger->trace("UI_button:UI_texture {}, UIFRame +{}+{}, {}x{}", name,
                          UIFrame.left, UIFrame.top, UIFrame.width, UIFrame.height);
    displayed = true;
    clickable = is_clickable;

    setAnimation(button_spritesheet);
}

// label with text
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::string ntext, std::shared_ptr<Animation> button_spritesheet) :
    UI_element(name, UIFrame, parent, resload, button_spritesheet), text_string(ntext)
{
    loading_logger->trace("UI_button:UI_label {}, UIFRame +{}+{}, {}x{}", name,
                          UIFrame.left, UIFrame.top, UIFrame.width, UIFrame.height);

    // default font
    setFont("Arial");
    // default char size
    setCharSize(24);
    // default color
    text_label.setFillColor(sf::Color::Black);

    text_label.setString(ntext);

    displayed = true;
}

// button with callback
UI_button::UI_button(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Animation> button_spritesheet, std::function<void()> ncallback) :
    UI_element(name, UIFrame, parent, resload, button_spritesheet), clickable(true)
{
    loading_logger->trace("UI_button:UI_button {}, UIFRame +{}+{}, {}x{}", name,
                          UIFrame.left, UIFrame.top, UIFrame.width, UIFrame.height);

    displayed = true;

    if (ncallback)
        add_callback(ncallback);
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

// block immune setter/getter
void UI_button::set_immune_to_controls_block(bool immune)
{
    ignore_controls_blocking = immune;
}

bool UI_button::get_immune_to_controls_block() const
{
    return ignore_controls_blocking;
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
void UI_button::push_click(sf::Vector2f cursor, bool controls_blocked)
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
    if (background_animation && clickable)
        set_current_frame(1);
}

// releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
void UI_button::release_click(sf::Vector2f cursor, bool controls_blocked, bool skip_action)
{
    input_logger->trace("Button {} released from {}x{}", name, cursor.x, cursor.y);

    pressed = false;

    // button logic
    if (clickable)
    {
        // changes texture, when <pressed> is changed
        // by default: 1 - pressed button, 0 - unpressed
        if (background_animation)
            set_current_frame(0);

        // if controls are blocked and this button is not immune
        if (controls_blocked && !ignore_controls_blocking)
            return;

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

// updates text coordinates considering alignment and font size
void UI_button::updateTextPosition()
{
    sf::Vector2f text_pos(Frame_scale.getPosition());
    switch (align)
    {
    case 0:
        break;
    case 1:
        text_pos += sf::Vector2f(Frame_scale.getSize());
        text_pos -= text_label.getLocalBounds().getSize();
    case 2:
        text_pos += sf::Vector2f((float)Frame_scale.width / 2, (float)Frame_scale.height / 2);
        sf::Vector2f local_size = text_label.getLocalBounds().getSize();
        text_pos -= sf::Vector2f(local_size.x / 2, local_size.y / 2);
    }
    
    text_label.setPosition(text_pos);
}

// setting font for displayed text
void UI_button::setFont(std::string fname)
{
    cur_font = resource_manager->getFont(fname);
    text_label.setFont(*cur_font);
    updateTextPosition();
}

void UI_button::setCharSize(unsigned int size)
{
    text_label.setCharacterSize(size);
    updateTextPosition();
}

// setting string for displayed text
void UI_button::setText(std::string ntext)
{
    text_string = ntext;
    text_label.setString(text_string);
    updateTextPosition();
}

std::string UI_button::getText() const
{
    return text_string;
}

// setting align, supports: "left", "centered", "right"
void UI_button::setAlign(std::string align_mode)
{
    if (align_mode == "left")
        align = 0;
    else if (align_mode == "right")
        align = 1;
    else if (align_mode == "center")
        align = 2;
    else
        loading_logger->warn("Unknown alignment type {}", align_mode);
    
    updateTextPosition();
}

// getting Text, for example, to calculate it's visual size
sf::Text& UI_button::getTextLabel()
{
    return text_label;
}

// override draw since we need to display text
void UI_button::draw(RenderTarget& target, RenderStates states) const
{
    UI_element::draw(target, states);

    if (parent_window)
        states.transform *= parent_window->getTransform();

    if (displayed && text_string != "")
    {
        target.draw(text_label, states);
    }
}