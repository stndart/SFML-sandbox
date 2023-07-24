#include "AnimatedSprite.h"

// creates ASrite with given shape and position. Shape can have texture with texcoords set
// shape is unchangeable after initialization
// accepts origin as well. Default it top-left
AnimatedSprite::AnimatedSprite(std::string name, std::unique_ptr<Shape> shape, Vector2f pos, Vector2f origin, int z_ind) : m_position(pos),
m_frameTime(seconds(0)), m_currentFrame(0), frame_stop_after(-1), m_isLooped(false), m_isReversed(false), m_isReversible(false),
parent_shape(std::move(shape)),
m_currentTime(seconds(0)), m_isPaused(true), passed_after_stop(seconds(0)),
name(name), z_index(z_ind)
{
    // Reaching out to global "loading" logger and "graphics" logger by names
    loading_logger = spdlog::get("loading");
    graphics_logger = spdlog::get("graphics");

    setPosition(m_position);
    setOrigin(origin);

    m_animation = std::shared_ptr<Animation>{nullptr};
}

// creates ASprite with rectangular shape of <posrect> size and position and <texrect> texture coordinates
AnimatedSprite::AnimatedSprite(std::string name, Texture* texture, IntRect texrect, FloatRect posrect, Vector2f origin, int z_ind) :
    AnimatedSprite(name, std::unique_ptr<Shape>(new RectangleShape(posrect.getSize())), posrect.getPosition(), origin, z_ind)
{
    // set to 0th frame and reset time
    setFrame(0, true);
}

// creates ASprite with given animation and parameters (frametime, paused, looped, reversible)
AnimatedSprite::AnimatedSprite(std::string name, std::shared_ptr<Animation> animation, FloatRect posrect, Vector2f origin,
               Time frameTime, bool looped, bool reversible, int z_ind) :
    AnimatedSprite(name, std::unique_ptr<Shape>(new RectangleShape(posrect.getSize())), posrect.getPosition(), origin, z_ind)
{
    if (animation->getSize() == 0)
    {
        loading_logger->error("Trying to construct ASprite with empty Animation");
        throw; /// ходят слухи, что throw в конструкторе - выстрел в ногу
    }

    m_animation = animation;
    m_frameTime = frameTime;
    m_isLooped = looped;
    m_isReversible = reversible;

    // set to 0th frame and reset time
    setFrame(0, true);
}

// Load animation, spritesheet and set current frame to 0
void AnimatedSprite::setAnimation(std::shared_ptr<Animation> animation)
{
    m_animation = animation;

    // set to 0th frame and reset time
    setFrame(0, true);
}

// for external uses after VisualEffect inheritance
std::shared_ptr<Animation> AnimatedSprite::getAnimation()
{
    return m_animation;
}

void AnimatedSprite::setFrameTime(Time time)
{
    m_frameTime = time;
}

void AnimatedSprite::play()
{
    graphics_logger->trace("play: frame={}, time {}", m_currentFrame, m_currentTime.asSeconds());

    m_isPaused = false;
}

// Play with offset. Use <shift> and <frame_start> to manage offset
void AnimatedSprite::play(std::size_t frame_start, Time shift)
{
    graphics_logger->trace("play with shift: {}", shift.asSeconds());

    setFrame(frame_start);
    m_currentTime = shift;
    play();
}

// set Animation, cur_time and then play
void AnimatedSprite::play(std::shared_ptr<Animation> animation, Time shift)
{
    graphics_logger->trace("play with shift: {}", shift.asSeconds());

    setAnimation(animation);
    m_currentTime = shift;
    play();
}

// set Animation, cur_frame, cur_time and then play
void AnimatedSprite::play(std::shared_ptr<Animation> animation, std::size_t frame_start, Time shift)
{
    graphics_logger->trace("play with shift: {}", shift.asSeconds());

    setAnimation(animation);
    setFrame(frame_start);
    m_currentTime = shift;
    play();
    //update();
}

void AnimatedSprite::pause()
{
    graphics_logger->trace("pause: frame={}, time {}", m_currentFrame, m_currentTime.asSeconds());

    m_isPaused = true;
}

// pause and reset animation timer (revert to first frame)
void AnimatedSprite::stop()
{
    graphics_logger->trace("stop: frame={}, time {}", m_currentFrame, m_currentTime.asSeconds());

    m_isPaused = true;
    m_currentFrame = 0;
    setFrame(m_currentFrame);
}

// sets frame to stop after. -1 means last frame
void AnimatedSprite::stop_after(int frame)
{
    if (!m_animation)
    {
        frame_stop_after = -1;
        return;
    }

    if (frame != -1)
        frame_stop_after = frame;
    else
        frame_stop_after = m_animation->getSize() - 1;

    // if asked to stop after x, then unloop
    setLooped(false);
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

        // set to new frame in spritesheet
        parent_shape->setTexture(m_animation->getTexture(m_currentFrame));
        parent_shape->setTextureRect(m_animation->getFrame(m_currentFrame));
    }

    if (resetTime)
        m_currentTime = seconds(0);
}

size_t AnimatedSprite::getFrame() const
{
    return m_currentFrame;
}

// paints texture with color ROFL; by default transparent
void AnimatedSprite::setColor(const Color& color)
{
    parent_shape->setFillColor(color);
}

// get time until animation ends. Ignores m_Looped. Takes m_Reversible and m_Reversed into account
Time AnimatedSprite::animation_remaining_time() const
{
    if (m_animation)
    {
        if (isReversed())
            return animation_remaining_time(frame_stop_after - 1);
        else
            return animation_remaining_time(frame_stop_after);
    }
    else
        return seconds(0);
}

Time AnimatedSprite::animation_remaining_time(size_t to_frame) const
{
    int remains = 0;
    if (m_animation && isPlaying())
    {
        if (m_isReversible)
        {
            if (m_isReversed)
            {
                if (m_currentFrame > to_frame)
                    remains = (m_currentFrame - to_frame) * m_frameTime.asMicroseconds() - m_currentTime.asMicroseconds();
                else
                    remains = -1;
            }
            else
            {
                if (m_currentFrame < to_frame)
                    remains = (to_frame - m_currentFrame) * m_frameTime.asMicroseconds() - m_currentTime.asMicroseconds();
                else
                    remains = (2 * m_animation->getSize() - to_frame - m_currentFrame) * m_frameTime.asMicroseconds() - m_currentTime.asMicroseconds();
            }
        }
        else
        {
            if (m_isReversed)
            {
                if (m_currentFrame > to_frame)
                    remains = (m_currentFrame - to_frame) * m_frameTime.asMicroseconds() - m_currentTime.asMicroseconds();
                else
                    remains = -1;
            }
            else
            {
                if (m_currentFrame < to_frame)
                    remains = (to_frame - m_currentFrame) * m_frameTime.asMicroseconds() - m_currentTime.asMicroseconds();
                else
                    remains = -1;
            }
        }
    }

    return microseconds(remains);
}

// for VisualEffect inheritance
Time AnimatedSprite::movement_remaining_time() const
{
    return seconds(0);
}

Time AnimatedSprite::time_after_stop() const
{
    return passed_after_stop;
}

void AnimatedSprite::move(const Vector2f &shift)
{
    Transformable::move(shift);
    parent_shape->move(shift);
}

void AnimatedSprite::rotate(float angle)
{
    Transformable::rotate(angle);
    parent_shape->rotate(angle);
}

void AnimatedSprite::scale(const Vector2f &factor)
{
    Transformable::scale(factor);
    parent_shape->scale(factor);
}

FloatRect AnimatedSprite::getLocalBounds() const
{
    return parent_shape->getLocalBounds();
}

FloatRect AnimatedSprite::getGlobalBounds() const
{
    return parent_shape->getGlobalBounds();
}

const Vector2f& AnimatedSprite::getPosition() const
{
    return Transformable::getPosition();
}

float AnimatedSprite::getRotation() const
{
    return Transformable::getRotation();
}

const Vector2f& AnimatedSprite::getScale() const
{
    return Transformable::getScale();
}

void AnimatedSprite::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    parent_shape->setPosition(position);
}

void AnimatedSprite::setPosition(float x, float y)
{
    Transformable::setPosition(x, y);
    parent_shape->setPosition(x, y);
}

void AnimatedSprite::setScale(const Vector2f &factors)
{
    Transformable::setScale(factors);
    parent_shape->setScale(factors);
}

void AnimatedSprite::setScale(float factorX, float factorY)
{
    Transformable::setScale(factorX, factorY);
    parent_shape->setScale(factorX, factorY);
}

void AnimatedSprite::setOrigin(const Vector2f &origin)
{
    Transformable::setOrigin(origin);
    parent_shape->setOrigin(origin);
}

void AnimatedSprite::setOrigin(float x, float y)
{
    Transformable::setOrigin(x, y);
    parent_shape->setOrigin(x, y);
}

bool AnimatedSprite::is_frame_valid(int newframe)
{
    // when no animation, then no frame is valid
    if (!m_animation)
    {
        return false;
    }
    else
    {
        // out of bounds error
        if (newframe < 0 && newframe >= (int)m_animation->getSize())
            return false;
        else if (frame_stop_after == -1)
            return true;
        else // if frame_stop_after is valid and frame is in bounds
        {
            if (!m_isReversed)
                return newframe <= frame_stop_after;
            else
                return newframe >= frame_stop_after;
        }
    }
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
            int nextFrame;
            if (!m_isReversed)
                nextFrame = m_currentFrame + 1;
            else
                nextFrame = m_currentFrame - 1;

            // if next frame index is valid
            if (is_frame_valid(nextFrame)) {
                m_currentFrame = nextFrame;
            }
            // animation has reached last frame
            else
            {
                // normal animation end
                if (frame_stop_after == -1)
                {
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
                            graphics_logger->trace("AS: last(first, since reversed) frame reached");
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
                            graphics_logger->trace("AS: last frame reached");
                            break;
                        }
                    }
                }
                // manual stop
                else
                {
                    passed_after_stop = m_currentTime;
                    pause();
                    break;
                }
            }

            // set the current frame, not reseting the time
            setFrame(m_currentFrame, false);
        }
    }
    else if (m_animation)
        passed_after_stop += deltaTime;

//    graphics_logger->trace("AS:{}:update Playing? {}, cur time {}, stop time {}, deltaTime {}", name,
//                           !m_isPaused, m_currentTime.asSeconds(), passed_after_stop.asSeconds(), deltaTime.asSeconds());
}

// standard draw method. Draws only if animation is valid
void AnimatedSprite::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(*parent_shape);
}
