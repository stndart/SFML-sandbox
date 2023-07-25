#ifndef VISUALEFFECT_INCLUDE
#define VISUALEFFECT_INCLUDE


#include <iostream>
#include <string>

#include "AnimatedSprite.h"

#include <SFML/System/Time.hpp>

using namespace sf;

// VisualEffect (referred as VE hereafter) is a superstructer over AnimatedSprite.
// If AnimatedSprite provides drawing shapes and dynamical textures, VE provides smooth movement, rotation, scaling and changing color above all
// VE contains AnimatedSprite inside and is stackable (VE contains VE, that contains VE, that contains AnimatedSprite)
// Although VE doen't have any Animations, it fully supports setAnimation and getAnimation, but transferring calls to underlying AnimatedSprite
// Some presets are available:
// 1. EMPTY: VE does nothing, but lasts 1 seconds
// 2. FADE: colors underlying AnimatedSprite in black over 2 seconds
// 3. RFADE: inverse effect to FADE
// 4. BARREL_ROLL: rotates AnimatedSprite by 360 degrees over 2 seconds

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

    VisualEffect(AnimatedSprite* sprite, Preset p = RFADE, Time offset = seconds(0), int z_ind = 0, sf::BlendMode blend_mode = sf::BlendAlpha);
    VisualEffect(AnimatedSprite* sprite, Time offset, Time duration, State start, State finish, int z_ind = 0, sf::BlendMode blend_mode = sf::BlendAlpha);
    VisualEffect(AnimatedSprite* sprite, Time offset, Time duration, Vector2f start_pos, Vector2f finish_pos, int z_ind = 0, sf::BlendMode blend_mode = sf::BlendAlpha);

    // overriding AnimatedSprite methods
    std::shared_ptr<Animation> getAnimation() override;
    void setAnimation(std::shared_ptr<Animation> animation) override;
    void play() override;
    void play(std::shared_ptr<Animation> animation, Time shift=seconds(0)) override;
    void pause() override;
    void stop() override;
    void setLooped(bool looped) override;
    void setColor(const Color& color) override;

    // overriding AnimatedSprite movement-related methods
    virtual void move(const Vector2f &offset) override;
    virtual void rotate(float angle) override;
    virtual void scale(const Vector2f &factor) override;
    void setPosition(const Vector2f &position) override;
    void setPosition(float x, float y) override;
    void setScale(const Vector2f &factors) override;
    void setScale(float x, float y) override;
    void setOrigin(const Vector2f &origin) override;
    void setOrigin(float x, float y) override;

    virtual bool isPlaying() const;
    virtual Time animation_remaining_time() const override;
    // overriding same method from AnimatedSprite. Now makes sense
    virtual Time movement_remaining_time() const override;

    // overriding Drawable methods
    virtual void update(Time deltaTime) override;
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // VISUALEFFECT_INCLUDE
