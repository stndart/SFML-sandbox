#ifndef VISUALEFFECT_INCLUDE
#define VISUALEFFECT_INCLUDE


#include <iostream>
#include <string>

#include "AnimatedSprite.h"

#include <SFML/System/Time.hpp>

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

Vector2f midspeed(State start, State finish, Time duration);
State midstate(State start, State finish, Time duration);
State curstate(State start, State finish, Time duration, Time now);

class VisualEffect : public AnimatedSprite
{
private:
    State start;
    State finish;
    // current state flows from start to finish
    State now;
public:
    // child sprite to take visual effect on
    AnimatedSprite *sprite;

    VisualEffect(AnimatedSprite *sprite, Preset p = RFADE, Time offset = seconds(0));
    VisualEffect(AnimatedSprite *sprite, Time offset, Time duration, State start, State finish);
    VisualEffect(AnimatedSprite* sprite, Time offset, Time duration, Vector2f start_pos, Vector2f finish_pos);

    // overriding AnimatedSprite methods
    Animation* getAnimation() override;
    void setAnimation(Animation& animation);
    void play() override;
    void play(Animation &animation, Time shift=seconds(0)) override;
    void pause() override;
    void stop() override;
    void setLooped(bool looped) override;
    void setColor(const Color& color) override;
    Vector2f getPosition() const override;
    void setPosition(const Vector2f &position) override;

    virtual bool isPlaying() const;
    virtual Time animation_remaining_time() const override;
    // overriding same method from AnimatedSprite. Now makes sense
    virtual Time movement_remaining_time() const override;

    // overriding Transformable methods
    void move(const Vector2f &shift) override;
    void rotate(float angle) override;
    void scale(const Vector2f &factor) override;

    // overriding Drawable methods
    virtual void update(Time deltaTime) override;
    virtual void redraw(RenderTarget& target, RenderStates states) const override;
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // VISUALEFFECT_INCLUDE
