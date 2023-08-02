#include "UI_element.h"

#include "Scene.h"

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Scene* parent) :
    Frame_scale(UIFrame), background_animation(std::shared_ptr<Animation>(nullptr)), cur_frame(-1),
    focus(false), parent_scene(parent),
    name(name), displayed(false), z_index(0)
{
    // Reaching out to global "loading" logger and "input" logger by names
    loading_logger = spdlog::get("loading");
    input_logger = spdlog::get("input");

    setFrame(UIFrame);
}

UI_element::UI_element(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<Animation> spritesheet) : UI_element(name, UIFrame, parent)
{
    setAnimation(spritesheet);
}

// Frame_scale setter/getter
void UI_element::setFrame(sf::IntRect new_frame_scale)
{
    if (background_animation && background_animation->getSize() > 0)
    {
        sf::Vector2f new_scale(
            (float)new_frame_scale.width / (float)background_animation->getFrame(0).width,
            (float)new_frame_scale.height / (float)background_animation->getFrame(0).height
        );

        setScale(new_scale);
    }

    sf::Vector2f new_position(new_frame_scale.left, new_frame_scale.top);
    new_position.x += getOrigin().x * getScale().x;
    new_position.y += getOrigin().y * getScale().y;

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

// mouse hover check
bool UI_element::contains(sf::Vector2f cursor) const
{
    bool res = false;

    // if not displayed, cursor ignores element
    if (displayed)
        res = Frame_scale.contains(sf::Vector2i(cursor));

//    input_logger->trace("Element {} at {}x{} contains? {}", name, cursor.x, cursor.y, res);
//    input_logger->trace("bc displayed? {} and frame +{}+{}, {}x{}", displayed, Frame_scale.left, Frame_scale.top, Frame_scale.width, Frame_scale.height);

    return res;
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


// overriding Transformable methods
void UI_element::move(const Vector2f &offset)
{
    Transformable::move(offset);
    background.move(offset);
    
    Frame_scale.left += offset.x;
    Frame_scale.top += offset.y;
}

void UI_element::scale(const Vector2f &factor)
{
    Transformable::scale(factor);
    background.scale(factor);

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

// standard draw method. Draws only if displayed
void UI_element::draw(RenderTarget& target, RenderStates states) const
{
    if (displayed && background.getTexture())
    {
        target.draw(background, states);
    }
}

// before drawing send itself to sort by z-index
void UI_element::draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap) const
{
    zmap[z_index].push_back(this);
}