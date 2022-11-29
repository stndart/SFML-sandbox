#include "VisualEffect.h"

VisualEffect::VisualEffect(AnimatedSprite* sprite, Preset p, Time offset) : AnimatedSprite("default veffect"),
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

VisualEffect::VisualEffect(AnimatedSprite* sprite, Time offset, Time m_duration, State start, State finish) : AnimatedSprite("default veffect"),
    start(start), finish(finish), sprite(sprite)
{
    duration = m_duration;
    m_currentTime = offset;
    now = start;
    //std::cout << name << " now owns " << sprite->name << std::endl;
}

VisualEffect::VisualEffect(AnimatedSprite* sprite, Time offset, Time m_duration, Vector2f start_pos, Vector2f finish_pos) : AnimatedSprite("default motion effect"),
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

void VisualEffect::play()
{
    m_isPaused = false;
    passed_after_stop = seconds(0);
    if (sprite)
        sprite->play();
}

void VisualEffect::play(Animation& animation)
{
    if (sprite)
        sprite->play(animation);
}

void VisualEffect::pause()
{
    std::cout << "VE pause\n";
    m_isPaused = true;
    if (sprite)
        sprite->pause();
}

void VisualEffect::stop()
{
    std::cout << "VE stop\n";
    m_isPaused = true;
    if (sprite)
        sprite->stop();
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

bool VisualEffect::isPlaying() const
{
    return !m_isPaused;
}

void VisualEffect::setPosition(const Vector2f &position)
{
    AnimatedSprite::setPosition(position);
    sprite->setPosition(position);
}

Animation* VisualEffect::getAnimation()
{
    if (sprite)
        return sprite->getAnimation();
    return NULL;
}

void VisualEffect::setAnimation(Animation& animation)
{
    sprite->setAnimation(animation);
}

Time VisualEffect::animation_remaining_time() const
{
    return sprite->animation_remaining_time();
}

Time VisualEffect::movement_remaining_time() const
{
    //std::cout << "movement remains: mcur " << m_currentTime.asMilliseconds() << " dur " << duration.asMilliseconds() << std::endl;
    Time remain = std::max(seconds(0), duration - m_currentTime);
    return std::max(remain, sprite->movement_remaining_time());
}

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

Vector2f VisualEffect::getPosition() const
{
    return sprite->getPosition();
}

void VisualEffect::update(Time deltaTime)
{
    //std::cout << "VE is paused? " << m_isPaused << std::endl;

    if (sprite)
        sprite->update(deltaTime);

    if (m_isPaused)
        passed_after_stop += deltaTime;
    else
    {
        m_currentTime += deltaTime;

        Time midTime = duration / 2.0f;

        if (m_currentTime < seconds(0))
            return;

        if (m_currentTime < duration)
        {
            State s;
            State mid = midstate(start, finish, duration);
            if (m_currentTime < midTime)
                s = curstate(start, mid, duration / 2.0f, m_currentTime);
            else
                s = curstate(mid, finish, duration / 2.0f, m_currentTime - midTime);

            move(-now.pos);
            rotate(-now.rotation);
            Vector2f tscale = Vector2f(1.0f / now.scale.x, 1.0f / now.scale.y);
            scale(tscale);

            now = s;

            move(now.pos);
            rotate(now.rotation);
            scale(now.scale);
            setColor(now.color);
        }
        else
        {
            move(-now.pos);
            rotate(-now.rotation);
            scale(Vector2f(1.0f / now.scale.x, 1.0f / now.scale.y));

            now = finish;

            move(now.pos);
            rotate(now.rotation);
            scale(now.scale);
            setColor(now.color);

            //std::cout << "VE paused by end\n";
            m_isPaused = true;

            passed_after_stop = m_currentTime - duration;
        }
    }
}

void VisualEffect::redraw(RenderTarget& target, RenderStates states) const
{
    if (sprite)
        sprite->redraw(target, states);
}

void VisualEffect::draw(RenderTarget& target, RenderStates states) const
{
    redraw(target, states);
}
