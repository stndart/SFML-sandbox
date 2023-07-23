#include "VisualEffect.h"

Vector2f midspeed(State start, State finish, Time duration)
{
    float T = duration.asMicroseconds();
    Vector2f a = (finish.speed - start.speed) / T;
    Vector2f avspeed = (start.speed + finish.speed) / 2.0f;
    Vector2f da = (finish.pos - start.pos - avspeed * T) / T / T * 4.0f;
    Vector2f midspeed = start.speed + (a + da) * T / 2.0f;
    return midspeed;
}

State midstate(State start, State finish, Time duration)
{
    State mid;
    mid.rotation = (start.rotation + finish.rotation) / 2.0f;
    mid.color = Color((start.color.r + finish.color.r) / 2.0f,
                          (start.color.g + finish.color.g) / 2.0f,
                          (start.color.b + finish.color.b) / 2.0f,
                          (start.color.a + finish.color.a) / 2.0f);
    mid.pos = (start.pos + finish.pos) / 2.0f;
    mid.speed = midspeed(start, finish, duration);
    mid.scale = (start.scale + finish.scale) / 2.0f;
    return mid;
}

State curstate(State start, State finish, Time duration, Time now)
{
    float b = (float)now.asMicroseconds() / duration.asMicroseconds();
    float a = 1.0f - b;

    State mid;
    mid.rotation = start.rotation * a + finish.rotation * b;
    mid.color = Color(start.color.r * a + finish.color.r * b,
                          start.color.g * a + finish.color.g * b,
                          start.color.b * a + finish.color.b * b,
                          start.color.a * a + finish.color.a * b);
    mid.speed = start.speed * a + finish.speed * b;
    mid.scale = start.scale * a + finish.scale * b;

    float T = now.asMicroseconds();
    Vector2f acc = (finish.speed - start.speed) / T;
    mid.pos = start.pos + start.speed * T + acc / 2.0f * T * T;

    return mid;
}

VisualEffect::VisualEffect(AnimatedSprite* sprite, Preset p, Time offset) :
    AnimatedSprite("default VEffect", std::make_unique<RectangleShape>(Vector2f(sprite->getGlobalBounds().getSize())), Vector2f(0, 0)),
    sprite(sprite)
{

    switch (p)
    {
    case EMPTY:
        start = {0, Color(255, 255, 255),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        finish = {0, Color(255, 255, 255),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        duration = seconds(1);
        break;
    case FADE:
        start = {0, Color(255, 255, 255),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        finish = {0, Color(255, 255, 255, 0),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        duration = seconds(2);
        break;
    case RFADE:
        start = {0, Color(255, 255, 255, 0),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        finish = {0, Color(255, 255, 255),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        duration = seconds(2);
        break;
    case BARREL_ROLL:
        start = {0, Color(255, 255, 255),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        finish = {360, Color(255, 255, 255),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        duration = seconds(2);
        break;
    }

    m_currentTime = offset;
    now = start;
}

VisualEffect::VisualEffect(AnimatedSprite* sprite, Time offset, Time m_duration, State start, State finish) :
    AnimatedSprite("default VEffect", std::make_unique<RectangleShape>(Vector2f(sprite->getGlobalBounds().getSize())), Vector2f(0, 0)),
    start(start), finish(finish), sprite(sprite)
{
    duration = m_duration;
    m_currentTime = offset;
    now = start;

    graphics_logger->debug("{} VE is now owned by {} ASprite", name, sprite->name);
}

VisualEffect::VisualEffect(AnimatedSprite* sprite, Time offset, Time m_duration, Vector2f start_pos, Vector2f finish_pos) :
    AnimatedSprite("default motion effect", std::make_unique<RectangleShape>(Vector2f(sprite->getGlobalBounds().getSize())), Vector2f(0, 0)),
    sprite(sprite)
{
    duration = m_duration;
    float T = duration.asMicroseconds();
    Vector2f speed = (finish_pos - start_pos) / T;
    start = {0, Color(255, 255, 255),
        start_pos, speed, Vector2f(1, 1)};
    finish = {0, Color(255, 255, 255),
        finish_pos, speed, Vector2f(1, 1)};
    now = start;
    m_currentTime = offset;
}


// for external uses after VisualEffect inheritance
std::shared_ptr<Animation> VisualEffect::getAnimation()
{
    if (sprite)
        return sprite->getAnimation();
    return NULL;
}

void VisualEffect::setAnimation(std::shared_ptr<Animation> animation)
{
    sprite->setAnimation(animation);
}

void VisualEffect::play()
{
    graphics_logger->trace("VE play");

    m_isPaused = false;
    passed_after_stop = seconds(0);
    if (sprite)
        sprite->play();
}

void VisualEffect::play(std::shared_ptr<Animation> animation, Time shift)
{
    graphics_logger->trace("VE play");

    if (sprite)
        sprite->play(animation, shift);
}

void VisualEffect::pause()
{
    graphics_logger->trace("VE pause");

    m_isPaused = true;
    // even if movement has stopped, we don't stop animation (leg movement)
//    if (sprite)
//        sprite->pause();
}

void VisualEffect::stop()
{
    graphics_logger->trace("VE stop");

    m_isPaused = true;
    // even if movement has stopped, we don't stop animation (leg movement)
//    if (sprite)
//        sprite->pause();
}

void VisualEffect::setLooped(bool looped)
{
    if (sprite)
        sprite->setLooped(looped);
}

void VisualEffect::setColor(const Color& color)
{
    if (sprite)
        sprite->setColor(color);
}

void VisualEffect::setPosition(const Vector2f &position)
{
    AnimatedSprite::setPosition(position);
    if (sprite)
        sprite->setPosition(position);
}

void VisualEffect::setPosition(float x, float y)
{
    AnimatedSprite::setPosition(x, y);
    if (sprite)
        sprite->setPosition(x, y);
}

void VisualEffect::setScale(const Vector2f &factors)
{
    AnimatedSprite::setScale(factors);
    if (sprite)
        sprite->setScale(factors);
}

void VisualEffect::setScale(float x, float y)
{
    AnimatedSprite::setScale(x, y);
    if (sprite)
        sprite->setScale(x, y);
}

void VisualEffect::setOrigin(const Vector2f &origin)
{
    AnimatedSprite::setOrigin(origin);
    if (sprite)
        sprite->setOrigin(origin);
}

void VisualEffect::setOrigin(float x, float y)
{
    AnimatedSprite::setOrigin(x, y);
    if (sprite)
        sprite->setOrigin(x, y);
}

bool VisualEffect::isPlaying() const
{
    return !m_isPaused;
}

Time VisualEffect::animation_remaining_time() const
{
    return sprite->animation_remaining_time();
}

Time VisualEffect::movement_remaining_time() const
{
    graphics_logger->trace("movement remains: mcur {}, dur {}", m_currentTime.asMilliseconds(), duration.asMilliseconds());

    Time remain = std::max(seconds(0), duration - m_currentTime);
    return std::max(remain, sprite->movement_remaining_time());
}

void VisualEffect::move(const Vector2f &shift)
{
    if (sprite)
        sprite->move(shift);
}

void VisualEffect::rotate(float angle)
{
    if (sprite)
        sprite->rotate(angle);
}

void VisualEffect::scale(const Vector2f &factor)
{
    if (sprite)
        sprite->scale(factor);
}

void VisualEffect::update(Time deltaTime)
{
    if (sprite)
        sprite->update(deltaTime);

    if (m_isPaused)
        passed_after_stop += deltaTime;
    // if moving, then move
    else
    {
        // change inner timer (used to shift frames) only if playing
        m_currentTime += deltaTime;

        // timestamp at movement middle
        Time midTime = duration / 2.0f;

        if (m_currentTime < seconds(0))
            return;

        if (m_currentTime < duration)
        {
            // new current state
            State s;
            // state at movement middle
            // If start and stop have zero speed, this comes in handy
            State mid = midstate(start, finish, duration);
            if (m_currentTime < midTime)
                s = curstate(start, mid, duration / 2.0f, m_currentTime);
            else
                s = curstate(mid, finish, duration / 2.0f, m_currentTime - midTime);

            // revert to zero
            move(-now.pos);
            rotate(-now.rotation);
            Vector2f tscale = Vector2f(1.0f / now.scale.x, 1.0f / now.scale.y);
            scale(tscale);

            // update state
            now = s;

            // set new coordinates, etc.
            move(now.pos);
            rotate(now.rotation);
            scale(now.scale);
            setColor(now.color);
        }
        else
        {
            // forcible arrive at end (if somehow now > finish)
            move(-now.pos);
            rotate(-now.rotation);
            scale(Vector2f(1.0f / now.scale.x, 1.0f / now.scale.y));

            now = finish;

            move(now.pos);
            rotate(now.rotation);
            scale(now.scale);
            setColor(now.color);

            // stop movement
            pause();

            // begin to charge penalties
            passed_after_stop = m_currentTime - duration;
        }
    }
}

void VisualEffect::draw(RenderTarget& target, RenderStates states) const
{
    if (sprite)
        sprite->draw(target, states);
}
