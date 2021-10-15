#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(std::string name, Time frameTime, bool paused, bool looped) : name(name),
    m_isPaused(paused), m_animation(NULL), m_frameTime(frameTime), m_currentFrame(0), m_isLooped(looped), m_texture(NULL)
{

}

AnimatedSprite::AnimatedSprite(std::string name, Texture& texture, IntRect frame0) : name(name),
    m_isPaused(true), m_frameTime(seconds(0.2f)), m_currentFrame(0), m_isLooped(true)
{
    m_texture = &texture;
    m_animation = new Animation();
    m_animation->addFrame(frame0);
    m_animation->setSpriteSheet(*m_texture);
    setFrame(m_currentFrame);
}

void AnimatedSprite::setAnimation(Animation& animation)
{
    m_animation = &animation;
    m_texture = m_animation->getSpriteSheet();
    m_currentFrame = 0;
    setFrame(m_currentFrame);
}

void AnimatedSprite::setFrameTime(Time time)
{
    m_frameTime = time;
}

void AnimatedSprite::play()
{
    m_isPaused = false;
}

void AnimatedSprite::play(Animation& animation)
{
    if (getAnimation() != &animation)
        setAnimation(animation);
    play();
}

void AnimatedSprite::pause()
{
    m_isPaused = true;
}

void AnimatedSprite::stop()
{
    m_isPaused = true;
    m_currentFrame = 0;
    setFrame(m_currentFrame);
}

void AnimatedSprite::setLooped(bool looped)
{
    m_isLooped = looped;
}

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

FloatRect AnimatedSprite::getLocalBounds() const
{
    return FloatRect(m_animation->getFrame(m_currentFrame));
}

FloatRect AnimatedSprite::getGlobalBounds() const
{
    return getTransform().transformRect(getLocalBounds());
}

bool AnimatedSprite::isLooped() const
{
    return m_isLooped;
}

bool AnimatedSprite::isPlaying() const
{
    return !m_isPaused;
}

Time AnimatedSprite::getFrameTime() const
{
    return m_frameTime;
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

void AnimatedSprite::setFrame(std::size_t newFrame, bool resetTime)
{
    if (m_animation)
    {
        //calculate new vertex positions and texture coordiantes
        IntRect rect = m_animation->getFrame(newFrame);

        m_vertices[0].position = Vector2f(0.f, 0.f);
        m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
        m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
        m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

        float left = static_cast<float>(rect.left) + 0.0001f;
        float right = left + static_cast<float>(rect.width);
        float top = static_cast<float>(rect.top);
        float bottom = top + static_cast<float>(rect.height);

        m_vertices[0].texCoords = Vector2f(left, top);
        m_vertices[1].texCoords = Vector2f(left, bottom);
        m_vertices[2].texCoords = Vector2f(right, bottom);
        m_vertices[3].texCoords = Vector2f(right, top);

        m_texture = m_animation->getSpriteSheet(m_animation->getTextureIndex(newFrame));
    }

    if (resetTime)
        m_currentTime = Time::Zero;
}

void AnimatedSprite::update(Time deltaTime)
{
    //std::cout << name << " update pos " << getPosition().x << " " << getPosition().y << std::endl;

    // if not paused and we have a valid animation
    if (!m_isPaused && m_animation)
    {
        // add delta time
        m_currentTime += deltaTime;

        // if current time is bigger then the frame time advance one frame
        while (m_currentTime >= m_frameTime)
        {
            // shift time by one frame, but keep the remainder
            m_currentTime = microseconds(m_currentTime.asMicroseconds() - m_frameTime.asMicroseconds());

            // get next Frame index
            if (m_currentFrame + 1 < m_animation->getSize()) {
                m_currentFrame++;
            }
            else
            {
                // animation has ended
                //m_currentFrame = 0; // reset to start

                if (!m_isLooped)
                {
                    m_isPaused = true;
                }

            }

            // set the current frame, not reseting the time
            setFrame(m_currentFrame, false);
        }
    }
}

void AnimatedSprite::redraw(RenderTarget& target, RenderStates states) const
{
    draw(target, states);
}

void AnimatedSprite::draw(RenderTarget& target, RenderStates states) const
{
    //std::cout << "Who asked " << name << " to draw?\n";
    //std::cout << name << " draw pos " << getPosition().x << " " << getPosition().y << std::endl;

    if (m_animation && m_texture)
    {
        states.transform *= getTransform();
        states.texture = m_texture;
        target.draw(m_vertices, 4, Quads, states);
    }
}

void AnimatedSprite::onClick(bool pressed)
{
    if (pressed)
    {
        play();
        std::cout << "push" << std::endl;
    }
    else
    {
        stop();
        std::cout << "unpush" << std::endl;
    }
}
