#ifndef VISUALEFFECT_INCLUDE
#define VISUALEFFECT_INCLUDE

#include <SFML/System/Time.hpp>

#include <iostream>
#include <string>

#include "AnimatedSprite.h"

using namespace sf;

enum Preset
{
    EMPTY,
    FADE,
    RFADE,
    BARREL_ROLL
};

struct State
{
    float rotation;
    Color color;
    Vector2f pos;
    Vector2f speed;
    Vector2f scale;

    bool operator == (const State& r) const
    {
        return (rotation == r.rotation) && (color == r.color) && (pos == r.pos) && (speed == r.speed) && (scale == r.scale);
    }

    bool operator != (const State& r) const
    {
        return !(operator == (r));
    }
};

class VisualEffect : public AnimatedSprite
{
private:
    Time duration;
    Time offset;
    virtual void draw(RenderTarget& target, RenderStates states) const;
    State start;
    State finish;
    State now;
    void move(const Vector2f &offset);
    void rotate(float angle);
    void scale(const Vector2f &factor);
public:
    AnimatedSprite *sprite;
    std::string name;
    VisualEffect(std::string name, AnimatedSprite *sprite, Preset p = RFADE, Time offset = seconds(0));
    VisualEffect(std::string name, AnimatedSprite *sprite, Time offset, Time duration, State start, State finish);
    const Animation* getAnimation() const;
    void play();
    void play(const Animation &animation);
    void pause();
    void stop();
    void setLooped(bool looped);
    void setColor(const Color& color);
    virtual void update(Time deltaTime);
    void redraw(RenderTarget& target, RenderStates states) const;
};

#endif // VISUALEFFECT_INCLUDE
