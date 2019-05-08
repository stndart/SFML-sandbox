#ifndef ANIMATEDSPRITE_INCLUDE
#define ANIMATEDSPRITE_INCLUDE

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

#include "Animation.h"

using namespace sf;

class AnimatedSprite : public Drawable, public Transformable
{
public:
    explicit AnimatedSprite(Time frameTime = seconds(0.2f), bool paused = false, bool looped = true);

    void update(Time deltaTime);
    void setAnimation(const Animation& animation);
    void setFrameTime(Time time);
    void play();
    void play(const Animation& animation);
    void pause();
    void stop();
    void setLooped(bool looped);
    void setColor(const Color& color);
    const Animation* getAnimation() const;
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;
    bool isLooped() const;
    bool isPlaying() const;
    Time getFrameTime() const;
    void setFrame(std::size_t newFrame, bool resetTime = true);

private:
    const Animation* m_animation;
    Time m_frameTime;
    Time m_currentTime;
    std::size_t m_currentFrame;
    bool m_isPaused;
    bool m_isLooped;
    const Texture* m_texture;
    Vertex m_vertices[4];

    virtual void draw(RenderTarget& target, RenderStates states) const;

};

#endif // ANIMATEDSPRITE_INCLUDE
