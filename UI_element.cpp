#include "UI_element.h"

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Scene* parent) :
    background(NULL), cur_frame(-1),
    focus(false), parent_scene(parent),
    name(name), displayed(false)
{
    // Reaching out to global "loading" logger and "input" logger by names
    loading_logger = spdlog::get("loading");
    input_logger = spdlog::get("input");

    setFrame(UIFrame);
    // by default origin is in center
    setOrigin(sf::Vector2f(UIFrame.width / 2., UIFrame.height / 2.));
}

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, Animation* spritesheet) : UI_element(name, UIFrame, parent)
{
    setAnimation(spritesheet);
}

// Frame_scale setter/getter
void UI_element::setFrame(sf::IntRect new_frame_scale)
{
    Frame_scale = new_frame_scale;

    setPosition(sf::Vector2f(new_frame_scale.left, new_frame_scale.top));
}
sf::IntRect UI_element::getFrame() const
{
    return Frame_scale;
}

// Animation setter
void UI_element::setAnimation(Animation* spritesheet)
{
    // still can be NULL if 2nd constructor is called
    if (spritesheet)
    {
        background = spritesheet;
        set_current_frame(0);
        setFrame(Frame_scale);
    }
}

// current frame setter/getter
void UI_element::set_current_frame(int new_frame)
{
    cur_frame = new_frame;

    if (background)
    {
        // here is the first time texture is enabled, so we need to choose it
        if (cur_frame == -1)
            cur_frame = 0;

        //calculate new vertex positions and texture coordinates
        sf::IntRect rect = getFrame();
        sf::IntRect texFrame = background->getFrame(cur_frame);
        cutout_texture_to_frame(m_vertices, rect, texFrame);

        loading_logger->trace("set_current_frame. display frame +{}+{}, {}x{}", Frame_scale.left, Frame_scale.top, Frame_scale.width, Frame_scale.height);
    }
}

sf::Texture* UI_element::getTexture() const
{
    return background->getTexture(cur_frame);
}

const sf::IntRect& UI_element::getTextureFrame() const
{
    return background->getFrame(cur_frame);
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

// mouse hover check
bool UI_element::contains(sf::Vector2f cursor) const
{
    bool res = false;

    // if not displayed, cursor ignores element
    if (displayed)
        res = Frame_scale.contains(sf::Vector2i(cursor + getOrigin()));

//    input_logger->trace("Element {} at {}x{} contains? {}", name, cursor.x, cursor.y, res);
//    input_logger->trace("bc displayed? {} and frame +{}+{}, {}x{}", displayed, Frame_scale.left, Frame_scale.top, Frame_scale.width, Frame_scale.height);

    return res;
}

// pushes hovered element
void UI_element::push_click(sf::Vector2f cursor)
{
    // pure virtual
    input_logger->debug("Element {} clicked at {}x{}", name, cursor.x, cursor.y);
}
// releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
void UI_element::release_click(sf::Vector2f cursor, bool skip_action)
{
    // pure virtual;
    input_logger->debug("Element {} popped at {}x{}", name, cursor.x, cursor.y);
}

void UI_element::move(const Vector2f &offset)
{
    Transformable::move(offset);
    Frame_scale.left += offset.x;
    Frame_scale.top += offset.y;
}

FloatRect UI_element::getLocalBounds() const
{
    return FloatRect(Frame_scale);
}

FloatRect UI_element::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}

void UI_element::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    Frame_scale.left = position.x;
    Frame_scale.top = position.y;
}

// standard draw method. Draws only if displayed
void UI_element::draw(RenderTarget& target, RenderStates states) const
{
    if (displayed && background)
    {
        states.transform *= getTransform();
        states.texture = background->getTexture(cur_frame);
        target.draw(m_vertices, 4, Quads, states);
    }
}
