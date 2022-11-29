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
    State start;
    State finish;
    State now;
public:
    AnimatedSprite *sprite;
    VisualEffect(AnimatedSprite *sprite, Preset p = RFADE, Time offset = seconds(0));
    VisualEffect(AnimatedSprite *sprite, Time offset, Time duration, State start, State finish);
    VisualEffect(AnimatedSprite* sprite, Time offset, Time duration, Vector2f start_pos, Vector2f finish_pos);
    void move(const Vector2f &shift) override;
    void rotate(float angle) override;
    void scale(const Vector2f &factor) override;
    Animation* getAnimation() override;
    void setAnimation(Animation& animation);
    void play() override;
    void play(Animation &animation) override;
    void pause() override;
    void stop() override;
    virtual bool isPlaying() const;
    void setLooped(bool looped) override;
    void setColor(const Color& color) override;
    void setPosition(const Vector2f &position) override;
    Vector2f getPosition() const override;
    virtual Time animation_remaining_time() const override;
    virtual Time movement_remaining_time() const override;

    virtual void update(Time deltaTime) override;
    virtual void redraw(RenderTarget& target, RenderStates states) const override;
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // VISUALEFFECT_INCLUDE
