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
    bool clickable = false;
    // flag if is currently press (to invoke callback, when unpressed)
    bool pressed = false;
    // if button is immune to controls block (for example, menu button)
    bool ignore_controls_blocking = false;
    // array of pairs: callback - delay. Call them and see, what happens
    std::vector<std::pair<std::function<void()>, sf::Time>> callbacks;

protected:
    // string of text to display
    std::string text_string;

    // align mode of text: 0 = left, 1 = right, 2 = centered
    int align;
    // pointer to loaded font
    std::shared_ptr<sf::Font> cur_font;
    // SFML Text structure with set parameters: text_string, position and font
    sf::Text text_label;

    // updates text coordinates considering alignment and font size
    void updateTextPosition();

public:
    UI_button(std::string name, sf::IntRect UIFrame, Scene *parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Texture> button_texture);                                        // label with texture
    UI_button(std::string name, sf::IntRect UIFrame, Scene *parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Animation> button_spritesheet, bool is_clickable = false);       // label with texture
    UI_button(std::string name, sf::IntRect UIFrame, Scene *parent, std::shared_ptr<ResourceLoader> resload, std::string ntext, std::shared_ptr<Animation> button_spritesheet = NULL);        // label with text
    UI_button(std::string name, sf::IntRect UIFrame, Scene *parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Animation> button_spritesheet, std::function<void()> ncallback); // button with callback

    // clickable setter/getter
    void set_clickable(bool is_clickable);
    bool is_clickable() const;

    // block immune setter/getter
    void set_immune_to_controls_block(bool immune);
    bool get_immune_to_controls_block() const;

    // adding callbacks
    void set_callbacks(std::vector<std::pair<std::function<void()>, sf::Time>> new_callback);
    void add_callback(std::function<void()> callback, sf::Time delay = seconds(0));

    // pushes hovered element
    void push_click(sf::Vector2f cursor, bool controls_blocked = false) override;
    // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
    void release_click(sf::Vector2f cursor, bool controls_blocked = false, bool skip_action = false) override;

    // setting font for displayed text
    void setFont(std::string fname);
    void setCharSize(unsigned int size);
    // setting string for displayed text
    void setText(std::string ntext);
    std::string getText() const;
    // setting align, supports: "left", "centered", "right"
    void setAlign(std::string align_mode);
    // getting Text, for example, to calculate it's visual size
    sf::Text& getTextLabel();

    // override draw since we need to display text
    void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // UI_BUTTON_H
