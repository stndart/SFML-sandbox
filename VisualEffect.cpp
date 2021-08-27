#include "VisualEffect.h"

VisualEffect::VisualEffect(std::string name, AnimatedSprite *sprite, Preset p, Time offset) : name(name),
    sprite(sprite), offset(offset)
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

    now = start;
}

VisualEffect::VisualEffect(std::string name, AnimatedSprite *sprite, Time offset, Time duration, State start, State finish) :
    sprite(sprite), offset(offset), duration(duration), start(start), finish(finish), name(name)
{
    now = start;
    std::cout << name << " now owns " << sprite->name << std::endl;
}

void VisualEffect::play()
{
    m_isPaused = false;
    if (sprite)
        sprite->play();
}

void VisualEffect::play(const Animation& animation)
{
    if (sprite)
        sprite->play(animation);
}

void VisualEffect::pause()
{
    m_isPaused = true;
    if (sprite)
        sprite->pause();
}

void VisualEffect::stop()
{
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

void VisualEffect::move(const Vector2f &offset)
{
    if (sprite)
        sprite->move(offset);
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


const Animation* VisualEffect::getAnimation() const
{
    if (sprite)
        return sprite->getAnimation();
    return NULL;
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
                          (start.color.b + finish.color.b) / 2.0f);
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
                          start.color.b * a + finish.color.b * b);
    mid.speed = start.speed * a + finish.speed * b;
    mid.scale = start.scale * a + finish.scale * b;

    float T = now.asMicroseconds();
    Vector2f acc = (finish.speed - start.speed) / T;
    mid.pos = start.pos + start.speed * T + acc / 2.0f * T * T;

    /*std::cout << start.speed.x << "," << start.speed.y << " " << finish.speed.x << "," << finish.speed.y << std::endl;
    std::cout << " " << acc.x << "," << acc.y << " " << now.asMicroseconds() << std::endl;
    std::cout << mid.pos.x - start.pos.x << "," << mid.pos.y - start.pos.y << std::endl;
    std::cout << start.pos.x << "," << start.pos.y << " " << mid.pos.x << "," << mid.pos.y << std::endl;
    std::cout << "---- " << std::endl;*/

    return mid;
}

void VisualEffect::update(Time deltaTime)
{
    if (!m_isPaused)
    {
        if (sprite)
            sprite->update(deltaTime);

        m_currentTime += deltaTime;

        Time midTime = offset + duration / 2.0f;
        if (m_currentTime > offset && m_currentTime < offset + duration)
        {
            State s;
            State mid = midstate(start, finish, duration);
            if (m_currentTime < midTime)
                s = curstate(start, mid, duration / 2.0f, m_currentTime - offset);
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
            //setColor(now.color);
        }
        if (m_currentTime >= offset + duration && now != finish)
        {
            move(-now.pos);
            rotate(-now.rotation);
            scale(Vector2f(1.0f / now.scale.x, 1.0f / now.scale.y));

            now = finish;

            move(now.pos);
            rotate(now.rotation);
            scale(now.scale);
            //setColor(now.color);
        }
    }
}

void VisualEffect::redraw(RenderTarget& target, RenderStates states) const
{
    if (sprite) {
        std::cout << name << " calls for " << sprite->name << std::endl;
        sprite->redraw(target, states);
    }
}

void VisualEffect::draw(RenderTarget& target, RenderStates states) const
{
    std::cout << "Who asked " << name << " to draw?\n";
    redraw(target, states);
}
