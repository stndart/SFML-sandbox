#include "UI_element.h"

#include "Scene.h"
#include "UI_window.h"

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload) :
    Frame_scale(UIFrame), background_animation(std::shared_ptr<Animation>(nullptr)), cur_frame(-1),
    parent_scene(parent), resource_manager(resload),
    name(name), z_index(0)
{
    // Reaching out to global "loading" logger and "input" logger by names
    loading_logger = spdlog::get("loading");
    input_logger = spdlog::get("input");

    setFrame(UIFrame);
}

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Texture> background) :
    UI_element(name, UIFrame, parent, resload)
{
    std::shared_ptr<Animation> back = std::make_shared<Animation>(background, true);
    setAnimation(back);
}

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, std::shared_ptr<Animation> spritesheet) :
    UI_element(name, UIFrame, parent, resload)
{
    setAnimation(spritesheet);
}

// Frame_scale setter/getter
void UI_element::setFrame(sf::IntRect new_frame_scale)
{
    if (background_animation && background_animation->getSize() > 0 && fit_to_background)
    {
        sf::Vector2f new_scale(
            (float)new_frame_scale.width / (float)background_animation->getFrame(0).width,
            (float)new_frame_scale.height / (float)background_animation->getFrame(0).height
        );

        setScale(new_scale);
    }
    else if (!fit_to_background)
    {
        background.setTextureRect(new_frame_scale);
    }

    sf::Vector2f new_position(new_frame_scale.left, new_frame_scale.top);
    new_position.x += getOrigin().x * getScale().x;
    new_position.y += getOrigin().y * getScale().y;

    Frame_scale = new_frame_scale;
    setPosition(new_position);
}

sf::IntRect UI_element::getFrame() const
{
    return Frame_scale;
}

// Animation setter
void UI_element::setAnimation(std::shared_ptr<Animation> spritesheet)
{
    // still can be NULL if 2nd constructor is called
    if (spritesheet)
    {
        background_animation = spritesheet;
        set_current_frame(0);
    }
}

// current frame setter/getter
void UI_element::set_current_frame(int new_frame)
{
    cur_frame = new_frame;

    if (background_animation)
    {
        // here is the first time texture is enabled, so we need to choose it
        if (cur_frame == -1)
            cur_frame = 0;
        
        background.setTexture(*background_animation->getTexture(cur_frame));
        background.setTextureRect(background_animation->getFrame(cur_frame));

        setFrame(Frame_scale);
    }
}

std::shared_ptr<sf::Texture> UI_element::getTexture() const
{
    return background_animation->getTexture(cur_frame);
}

const sf::IntRect& UI_element::getTextureFrame() const
{
    return background_animation->getFrame(cur_frame);
}

// focus setter/getter
void UI_element::set_focus(bool new_focus)
{
    focus = new_focus;

    // when losing focus, release click
    if (!focus)
    {
        release_click(sf::Vector2f(0, 0), true);
    }
}

bool UI_element::is_focused() const
{
    return focus;
}

// sets displayed for self (and child elements)
void UI_element::show(bool disp)
{
    displayed = disp;
    // if not displayed, then cursor effectively wandered off
    // if just displayed, refresh all hover timers
    hover_off();
}

// mouse hover check
bool UI_element::contains(sf::Vector2f cursor) const
{
    bool res = false;

    // if not displayed, cursor ignores element
    if (displayed)
    {
        sf::Vector2f parent_coords(0, 0);
        if (parent_window)
            parent_coords = parent_window->getTransform().transformPoint(parent_coords);
        res = Frame_scale.contains(sf::Vector2i(cursor - parent_coords));
    }

    return res;
}

// set parent window to support hints, popups, etc.
void UI_element::set_parent_window(UI_window* pwindow)
{
    parent_window = pwindow;
}

// pushes hovered element
void UI_element::push_click(sf::Vector2f cursor, bool controls_blocked)
{
    // pure virtual
    input_logger->debug("Element {} clicked at {}x{}", name, cursor.x, cursor.y);
}
// releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
void UI_element::release_click(sf::Vector2f cursor, bool controls_blocked, bool skip_action)
{
    // pure virtual;
    input_logger->debug("Element {} popped at {}x{}", name, cursor.x, cursor.y);
}

// if mouse in hovering the element
bool UI_element::is_hovered() const
{
    return hovered;
}

// hoveres element under cursor.
void UI_element::hover_on(sf::Vector2f cursor)
{
    if (hoverable)
        if (contains(cursor))
            hovered = true;
}

// lifts hover under cursor
void UI_element::hover_off()
{
    if (hoverable)
    {
        hovered = false;
        parent_scene->show_UI_window(name + ":hint", false);
        on_hover = sf::seconds(0);
    }
}

// sets hoverable to self and children
void UI_element::set_hoverable(bool hover)
{
    hoverable = hover;
}

// overriding some Sprite methods
void UI_element::setColor(const Color& color)
{
    background.setColor(color);
}

// overriding Transformable methods
void UI_element::move(const Vector2f &offset)
{
    setPosition(getPosition() + offset);
}

void UI_element::scale(const Vector2f &factor)
{
    sf::Vector2f new_scale = getScale();
    new_scale.x *= factor.x;
    new_scale.y *= factor.y;
    setScale(new_scale);
}

FloatRect UI_element::getLocalBounds() const
{
    return FloatRect(Frame_scale);
}

FloatRect UI_element::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}

// changes Frame_scale topleft since origin stays unchanged
void UI_element::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    background.setPosition(position);

    // update FrameScale from original sprite size and its current scale
    Frame_scale.left = getPosition().x - getOrigin().x * getScale().x;
    Frame_scale.top = getPosition().y - getOrigin().y * getScale().y;
}

void UI_element::setPosition(float x, float y)
{
    setPosition(Vector2f(x, y));
}

// changes Frame_scale topleft since position stays unchanged
void UI_element::setOrigin(const Vector2f &origin)
{
    Transformable::setOrigin(origin);
    background.setOrigin(origin);

    // update FrameScale from original sprite size and its current scale
    Frame_scale.left = getPosition().x - getOrigin().x * getScale().x;
    Frame_scale.top = getPosition().y - getOrigin().y * getScale().y;
}

void UI_element::setOrigin(float x, float y)
{
    setOrigin(Vector2f(x, y));
}

void UI_element::setScale(const Vector2f &factors)
{
    Transformable::setScale(factors);
    background.setScale(factors);

    // update FrameScale from original sprite size and its current scale
    Vector2f new_pos(
        getPosition().x - getOrigin().x * getScale().x,
        getPosition().y - getOrigin().y * getScale().y
    );

    Vector2f new_size(background.getTextureRect().getSize());
    new_size.x *= getScale().x;
    new_size.y *= getScale().y;

    Frame_scale = IntRect(Vector2i(new_pos), Vector2i(new_size));
}

void UI_element::setScale(float factorX, float factorY)
{
    setScale(Vector2f(factorX, factorY));
}

// handling mouse movements
void UI_element::update(Event& event)
{
    if (event.type == sf::Event::MouseMoved)
    {
        // hides hint, when mouse moves
        // hover_on(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
        // hover_off();
    }
}

void UI_element::update(sf::Time deltatime)
{
    if (displayed && hoverable && hovered)
        on_hover += deltatime;

    if (hoverable && hovered && on_hover > hover_min)
    {
        sf::Vector2f hint_pos = sf::Vector2f(Frame_scale.getPosition());
        hint_pos += sf::Vector2f(Frame_scale.getSize());

        std::string hint_window_name = name + ":hint";
        
        bool just_created = true;
        std::shared_ptr<UI_window> hint_window = parent_scene->create_subwindow_dont_register(hint_window_name, "hint");
        // if parent window (which should, if exists) contains "new" window, then it is not new
        if (parent_window && parent_window->get_subwindow(hint_window_name))
            just_created = false;
        // if parent window doesn't exist, then <this> is top level <Interface> and should contain "new" window itself
        // but base interface should not create hints, so just leave this window empty
        else if (!parent_window)
            just_created = false;

        sf::IntRect UIWindowFrame = sf::IntRect(sf::Vector2i(hint_pos), sf::Vector2i(Frame_scale.getSize()));
        sf::IntRect UIHintFrame = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(Frame_scale.getSize()));
        if (!hint_window)
        {
            hint_window = std::make_shared<UI_window>(hint_window_name, UIWindowFrame, parent_scene, resource_manager);
        }

        if (just_created)
        {
            std::shared_ptr<UI_button> hint_label = std::make_shared<UI_button>(name + ":hint_label", UIHintFrame, parent_scene, resource_manager, name);

            hint_window->addElement(hint_label, 3);
            hint_window->set_hoverable(false);

            sf::Vector2i newsize(hint_label->getTextLabel().getGlobalBounds().getSize());
            UIHintFrame = sf::IntRect(UIHintFrame.getPosition(), newsize);
            UIWindowFrame = sf::IntRect(UIWindowFrame.getPosition(), newsize);
            
            hint_label->setFrame(UIHintFrame);
            hint_window->setFrame(UIWindowFrame);
            hint_label->getTextLabel().move(-hint_label->getTextLabel().getLocalBounds().getPosition());
        }

        if (parent_window)
            parent_window->addElement(hint_window, 3);
        else
            parent_scene->add_UI_element(hint_window, 3);
    }
}

// standard draw method. Draws only if displayed
void UI_element::draw(RenderTarget& target, RenderStates states) const
{
    // sf::Vector2f parent_coords(0, 0);
    if (parent_window)
        states.transform *= parent_window->getTransform();

    if (displayed && background.getTexture())
    {
        target.draw(background, states);
    }
}

// before drawing send itself to sort by z-index
void UI_element::draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap, int z_shift) const
{
    zmap[z_index + z_shift].push_back(this);
}