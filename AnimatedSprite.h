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

class AnimatedSprite : public Drawable, public Transformable
{
public:
    std::string name; /// TEMP

    explicit AnimatedSprite(std::string name, Time frameTime = seconds(0.2f), bool paused = false, bool looped = true, bool reversible = false);
    AnimatedSprite(std::string name, Texture& texture, IntRect frame0);

    void setAnimation(Animation& animation);
    void setFrameTime(Time time);
    virtual void play();
    virtual void play(Animation& animation);
    virtual void pause();
    virtual void stop();
    virtual void setLooped(bool looped);
    virtual void setReversible(bool reversible);
    virtual void setReversed(bool reversed);
    virtual void setColor(const Color& color);
    virtual Animation* getAnimation();
    virtual void move(const Vector2f &offset);
    virtual void rotate(float angle);
    virtual void scale(const Vector2f &factor);
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;
    virtual bool isLooped() const;
    virtual bool isReversed() const;
    virtual bool isReversible() const;
    virtual bool isPlaying() const;
    virtual Time getFrameTime() const;
    virtual void setFrame(std::size_t newFrame, bool resetTime = true);
    virtual void setPosition(const Vector2f &position);
    virtual Vector2f getPosition() const;
    virtual Time animation_remaining_time() const;
    virtual Time movement_remaining_time() const;

    virtual void update(Time deltaTime);
    virtual void redraw(RenderTarget& target, RenderStates states) const;
    virtual void draw(RenderTarget& target, RenderStates states) const override;

    void onClick(bool pressed);

protected:
    Time m_currentTime;
    bool m_isPaused;

private:
    Animation* m_animation;
    Time m_frameTime; //time for one frame
    std::size_t m_currentFrame;
    // replays animation from the beginning after the end is reached
    bool m_isLooped;
    // if animation is played backwards
    bool m_isReversed;
    // animation extends after its end with itself backwards.
    // m_isLooped in this case indicates if animation sways once or multiple times
    bool m_isReversible;
    Texture* m_texture;
    Vertex m_vertices[4];
};

#endif // ANIMATEDSPRITE_INCLUDE
