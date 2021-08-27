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
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    State start;
    State finish;
    State now;
public:
    AnimatedSprite *sprite;
    //std::string name;
    VisualEffect(std::string name, AnimatedSprite *sprite, Preset p = RFADE, Time offset = seconds(0));
    VisualEffect(std::string name, AnimatedSprite *sprite, Time offset, Time duration, State start, State finish);
    void move(const Vector2f &offset) override;
    void rotate(float angle) override;
    void scale(const Vector2f &factor) override;
    const Animation* getAnimation() const override;
    void play() override;
    void play(const Animation &animation) override;
    void pause() override;
    void stop() override;
    void setLooped(bool looped) override;
    void setColor(const Color& color) override;
    virtual void update(Time deltaTime) override;
    virtual void redraw(RenderTarget& target, RenderStates states) const override;
};

#endif // VISUALEFFECT_INCLUDE
