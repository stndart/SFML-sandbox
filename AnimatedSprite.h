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

    explicit AnimatedSprite(std::string name, Time frameTime = seconds(0.2f), bool paused = false, bool looped = true);
    AnimatedSprite(std::string name, Texture& texture, IntRect frame0);

    virtual void update(Time deltaTime);
    void setAnimation(Animation& animation);
    void setFrameTime(Time time);
    virtual void play();
    virtual void play(Animation& animation);
    virtual void pause();
    virtual void stop();
    virtual void setLooped(bool looped);
    virtual void setColor(const Color& color);
    virtual Animation* getAnimation();
    virtual void move(const Vector2f &offset);
    virtual void rotate(float angle);
    virtual void scale(const Vector2f &factor);
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;
    bool isLooped() const;
    bool isPlaying() const;
    Time getFrameTime() const;
    void setFrame(std::size_t newFrame, bool resetTime = true);
    virtual void redraw(RenderTarget& target, RenderStates states) const;

protected:
    Time m_currentTime;
    bool m_isPaused;

private:
    Animation* m_animation;
    Time m_frameTime;
    std::size_t m_currentFrame;
    bool m_isLooped;
    Texture* m_texture;
    Vertex m_vertices[4];

    virtual void draw(RenderTarget& target, RenderStates states) const override;

};

#endif // ANIMATEDSPRITE_INCLUDE
