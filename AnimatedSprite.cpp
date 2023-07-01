#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(std::string name, Time frameTime, bool paused, bool looped, bool reversible) :
    m_texture(NULL), m_animation(NULL),
    m_frameTime(frameTime), m_currentFrame(0),
    m_isLooped(looped), m_isReversed(false), m_isReversible(reversible),
    m_currentTime(seconds(0)), m_isPaused(paused), duration(seconds(0)), passed_after_stop(seconds(0)),
    name(name)
{

}

AnimatedSprite::AnimatedSprite(std::string name, Texture& texture, IntRect frame0) :
    m_frameTime(seconds(0.2f)), m_currentFrame(0),
    m_isLooped(true), m_isReversed(false), m_isReversible(false),
    m_currentTime(seconds(0)), m_isPaused(true), duration(seconds(0)), passed_after_stop(seconds(0)),
    name(name)
{

    m_texture = &texture;
    m_animation = new Animation();
    m_animation->addFrame(frame0);
    m_animation->setSpriteSheet(*m_texture);
    setFrame(m_currentFrame);
}

// Load animation, spritesheet and set current frame to 0
void AnimatedSprite::setAnimation(Animation& animation)
{
    m_animation = &animation;
    m_texture = m_animation->getSpriteSheet();
    m_currentFrame = 0;
    setFrame(m_currentFrame);

    duration = m_frameTime * (float)m_animation->getSize();
}

void AnimatedSprite::setFrameTime(Time time)
{
    m_frameTime = time;
}

void AnimatedSprite::play()
{
//    std::cout << "play: frame " << m_currentFrame << " time " << m_currentTime.asSeconds() << std::endl;
    m_isPaused = false;
}

// set Animation and then play
void AnimatedSprite::play(Animation& animation, Time shift)
{
//    std::cout << "play with shift: " << shift.asSeconds() << std::endl;
    setAnimation(animation);
    m_currentTime = shift;
    play();
    //update();
}

void AnimatedSprite::pause()
{
//    std::cout << "pause: frame" << m_currentFrame << " time " << m_currentTime.asSeconds() << std::endl;
    m_isPaused = true;
}

// pause and reset animation timer (revert to first frame)
void AnimatedSprite::stop()
{
//    std::cout << "stop: frame" << m_currentFrame << " time " << m_currentTime.asSeconds() << std::endl;
    m_isPaused = true;
    m_currentFrame = 0;
    setFrame(m_currentFrame);
}

// replays animation from the beginning after the end is reached
void AnimatedSprite::setLooped(bool looped)
{
    m_isLooped = looped;
}

// is animation played backwards
void AnimatedSprite::setReversed(bool reversed)
{
    m_isReversed = reversed;
}

// animation extends after its end with itself everted backwards.
// m_isLooped in this case indicates if animation sways once or multiple times
void AnimatedSprite::setReversible(bool reversible)
{
    m_isReversible = reversible;
}

// paints texture with color ROFL; by default transparent
void AnimatedSprite::setColor(const Color& color)
{
    // Update the vertices' color
    m_vertices[0].color = color;
    m_vertices[1].color = color;
    m_vertices[2].color = color;
    m_vertices[3].color = color;
}

Animation* AnimatedSprite::getAnimation()
{
    return m_animation;
}

void AnimatedSprite::move(const Vector2f &offset)
{
    Transformable::move(offset);
}

void AnimatedSprite::rotate(float angle)
{
    Transformable::rotate(angle);
}

void AnimatedSprite::scale(const Vector2f &factor)
{
    Transformable::scale(factor);
}

FloatRect AnimatedSprite::getLocalBounds() const
{
    return FloatRect(m_animation->getFrame(m_currentFrame));
}

FloatRect AnimatedSprite::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}

Vector2f AnimatedSprite::getPosition() const
{
    return Transformable::getPosition();
}

void AnimatedSprite::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
}

void AnimatedSprite::setScale(const Vector2f &factors)
{
    Transformable::setScale(factors);
}

void AnimatedSprite::setScale(float factorX, float factorY)
{
    Transformable::setScale(factorX, factorY);
}

bool AnimatedSprite::isLooped() const
{
    return m_isLooped;
}

bool AnimatedSprite::isReversed() const
{
    return m_isReversed;
}

bool AnimatedSprite::isReversible() const
{
    return m_isReversible;
}

bool AnimatedSprite::isPlaying() const
{
    return !m_isPaused;
}

Time AnimatedSprite::getFrameTime() const
{
    return m_frameTime;
}

// Set current frame by index and reset m_currentTime - frame time (if resetTime)
// Current frame remains unchanged until m_currentTime exceeds m_frameTime
void AnimatedSprite::setFrame(std::size_t newFrame, bool resetTime)
{
    if (m_animation)
    {
        m_currentFrame = newFrame;
        //calculate new vertex positions and texture coordiantes
        IntRect rect = m_animation->getFrame(newFrame);

        // Where to draw texture (relative to sprite anchor)
        m_vertices[0].position = Vector2f(0.f, 0.f);
        m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
        m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
        m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

        float left = static_cast<float>(rect.left) + 0.0001f;
        float right = left + static_cast<float>(rect.width);
        float top = static_cast<float>(rect.top);
        float bottom = top + static_cast<float>(rect.height);

        // What to draw (coordinate on texture)
        m_vertices[0].texCoords = Vector2f(left, top);
        m_vertices[1].texCoords = Vector2f(left, bottom);
        m_vertices[2].texCoords = Vector2f(right, bottom);
        m_vertices[3].texCoords = Vector2f(right, top);

        // Ensure correct texture is selected
        m_texture = m_animation->getSpriteSheet(m_animation->getTextureIndex(newFrame));
    }

    if (resetTime)
        m_currentTime = Time::Zero;
}

Time AnimatedSprite::time_after_stop() const
{
    return passed_after_stop;
}

// whole animation duration
Time AnimatedSprite::get_duration() const
{
    return duration;
}

// get time until animation ends. Ignores m_Looped. Takes m_Reversible and m_Reversed into account
/// Not tested after repair
Time AnimatedSprite::animation_remaining_time() const
{
    int remains = 0;
    if (m_animation && isPlaying())
    {
        int full_time = m_frameTime.asMicroseconds() * m_animation->getSize();
        if (m_isReversible)
            full_time *= 2;
        int cur_time = m_currentTime.asMicroseconds() % full_time;
        if (!m_isReversed)
            cur_time += m_frameTime.asMicroseconds() * m_currentFrame;
        else if (m_isReversible) {
            cur_time += m_frameTime.asMicroseconds() * m_animation->getSize();
            cur_time += m_frameTime.asMicroseconds() * (m_animation->getSize() - m_currentFrame - 1);
        }
        remains += full_time - cur_time;
    }

    return microseconds(remains);
}

// for VisualEffect inheritance
Time AnimatedSprite::movement_remaining_time() const
{
    return seconds(0);
}

void AnimatedSprite::update(Time deltaTime)
{
    // if not paused and we have a valid animation
    if (!m_isPaused && m_animation)
    {
        // add delta time
        m_currentTime += deltaTime;
        passed_after_stop = seconds(0);

        // if current time is bigger then the frame time, then advance one frame
        while (m_currentTime >= m_frameTime)
        {
            // shift time by one frame back, but keep the remainder
            m_currentTime = microseconds(m_currentTime.asMicroseconds() - m_frameTime.asMicroseconds());

            // check if shifting frames makes sense
            if (m_animation->getSize() < 2)
            {
                break;
            }
            // get next frame index
            size_t nextFrame;
            if (!m_isReversed)
                nextFrame = m_currentFrame + 1;
            else
                nextFrame = m_currentFrame - 1;

            // if next frame index is valid
            if (nextFrame < m_animation->getSize() && nextFrame >= 0) {
                m_currentFrame = nextFrame;
            }
            else
            {
                // animation has reached last frame
                if (m_isReversed)
                {
                    if (m_isLooped)
                    {
                        // since frame 0 is already displayed and we need to reverse
                        m_currentFrame = 1;
                        m_isReversed = false;
                    }
                    else // stop animation
                    {
                        m_isReversed = false;
                        passed_after_stop = m_currentTime;
                        stop();
                        //std::cout << "last first frame reached\n";
                        break;
                    }
                }
                else
                {
                    if (m_isReversible)
                    {
                        // since last frame is already displayed and we need to reverse
                        m_currentFrame = m_animation->getSize() - 2;
                        m_isReversed = true;
                    }
                    else if (m_isLooped)
                    {
                        m_currentFrame = 0;
                    }
                    else
                    {
                        passed_after_stop = m_currentTime;
                        pause();
                        //std::cout << "last frame reached\n";
                        break;
                    }
                }
            }

            // set the current frame, not reseting the time
            setFrame(m_currentFrame, false);
        }
    }
    else if (m_animation)
        passed_after_stop += deltaTime;
}

void AnimatedSprite::redraw(RenderTarget& target, RenderStates states) const
{
    draw(target, states);
}

// standard draw method. Draws only if animation is valid
void AnimatedSprite::draw(RenderTarget& target, RenderStates states) const
{
    if (m_animation && m_texture)
    {
        states.transform *= getTransform();
        states.texture = m_texture;
        target.draw(m_vertices, 4, Quads, states);
    }
}

// stops and plays animation (for buttons)
void AnimatedSprite::onClick(bool pressed)
{
    if (pressed)
    {
        play();
    }
    else
    {
        stop();
    }
}
