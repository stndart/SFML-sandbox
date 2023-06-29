#ifndef ANIMATEDSPRITE_INCLUDE
#define ANIMATEDSPRITE_INCLUDE

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

#include <iostream>
#include <string>

#include "Animation.h"

using namespace sf;

// AnimatedSprite is drawable sprite, that can play and switch animations. Updated with deltatime, it automatically switches frames of Animation
class AnimatedSprite : public Drawable, public Transformable
{
public:
    std::string name; /// TEMP

    explicit AnimatedSprite(std::string name, Time frameTime = seconds(0.2f), bool paused = false, bool looped = true, bool reversible = false);
    AnimatedSprite(std::string name, Texture& texture, IntRect frame0);

    void setAnimation(Animation& animation);
    // set time for which each frame lasts
    void setFrameTime(Time time);
    virtual void play();
    // set Animation and then play
    virtual void play(Animation& animation);
    virtual void pause();
    // pause and reset animation timer (revert to first frame)
    virtual void stop();
    // replays animation from the beginning after the end is reached
    virtual void setLooped(bool looped);
    // is animation played backwards
    virtual void setReversed(bool reversed);
    // animation extends after its end with itself everted backwards.
    // m_isLooped in this case indicates if animation sways once or multiple times
    virtual void setReversible(bool reversible);
    // paints texture with color ROFL; by default transparent
    virtual void setColor(const Color& color);
    virtual Animation* getAnimation();

    // overriding Transformable methods
    virtual void move(const Vector2f &offset);
    virtual void rotate(float angle);
    virtual void scale(const Vector2f &factor);
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;
    virtual void setPosition(const Vector2f &position);
    virtual Vector2f getPosition() const;

    virtual bool isLooped() const;
    virtual bool isReversed() const;
    virtual bool isReversible() const;
    virtual bool isPlaying() const;
    virtual Time getFrameTime() const;
    virtual void setFrame(std::size_t newFrame, bool resetTime = true);

    // whole animation duration
    Time get_duration() const;
    Time time_after_stop() const;

    virtual Time animation_remaining_time() const;
    // not implemented
    virtual Time movement_remaining_time() const;

    // overriding Drawable methods
    virtual void update(Time deltaTime);
    virtual void redraw(RenderTarget& target, RenderStates states) const;
    virtual void draw(RenderTarget& target, RenderStates states) const override;

    // onclick action
    void onClick(bool pressed);
protected:
    // Inner time counter. Used to switch between frames
    Time m_currentTime;
    bool m_isPaused;
    // Total animation duration
    Time duration;
    Time passed_after_stop; /// to implement

private:
    Animation* m_animation;
    Time m_frameTime; //time for one frame
    std::size_t m_currentFrame; // current frame index
    bool m_isLooped;
    bool m_isReversed;
    bool m_isReversible;
    Texture* m_texture; // pointer to current spritesheet
    Vertex m_vertices[4]; // coordinates of frame on spritesheet
};

#endif // ANIMATEDSPRITE_INCLUDE
