#include "UI_element.h"

UI_element::UI_element(std::string name, sf::IntRect UIFrame) :
    background(NULL), cur_frame(-1),
    focus(false),
    name(name), displayed(false)
{
    loading_logger = spdlog::get("loading");
    input_logger = spdlog::get("input");

    setFrame(UIFrame);
    // by default origin is in center
    setOrigin(sf::Vector2f(UIFrame.width / 2., UIFrame.height / 2.));
}

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Animation* spritesheet) : UI_element(name, UIFrame)
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

        //calculate new vertex positions and texture coordiantes
        sf::IntRect rect = getFrame();

        loading_logger->trace("set_current_frame. display frame +{}+{}, {}x{}", Frame_scale.left, Frame_scale.top, Frame_scale.width, Frame_scale.height);

        // Where to draw texture (relative to sprite anchor)
        m_vertices[0].position = Vector2f(0.f, 0.f);
        m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
        m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
        m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

        sf::IntRect texFrame = background->getFrame(cur_frame);

        loading_logger->trace("set_current_frame. texture frame +{}+{}, {}x{}", texFrame.left, texFrame.top, texFrame.width, texFrame.height);

        float left = static_cast<float>(texFrame.left);/// + 0.0001f;
        float right = left + static_cast<float>(texFrame.width);
        float top = static_cast<float>(texFrame.top);
        float bottom = top + static_cast<float>(texFrame.height);

        // What to draw (coordinate on texture)
        m_vertices[0].texCoords = Vector2f(left, top);
        m_vertices[1].texCoords = Vector2f(left, bottom);
        m_vertices[2].texCoords = Vector2f(right, bottom);
        m_vertices[3].texCoords = Vector2f(right, top);
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
