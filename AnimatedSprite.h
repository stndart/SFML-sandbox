#ifndef ANIMATEDSPRITE_INCLUDE
#define ANIMATEDSPRITE_INCLUDE

#include <iostream>
#include <string>

#include "Animation.h"
#include "extra_algorithms.h"

#include <spdlog/spdlog.h>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

using namespace sf;

// AnimatedSprite is drawable sprite, that can play and switch animations. Updated with deltatime, it automatically switches frames of Animation
class AnimatedSprite : public Drawable, public Transformable
{

private:
     // pointer to current spritesheet
    Texture* m_texture;
    // coordinates of frame on spritesheet
    Vertex m_vertices[4];

    Animation* m_animation;
    //time for one frame
    Time m_frameTime;
    // current frame index
    std::size_t m_currentFrame;
    // index of frame to stop after (by default size - 1)
    std::size_t frame_stop_after;

    bool m_isLooped;
    bool m_isReversed;
    bool m_isReversible;

protected:
    // Inner time counter. Used to switch between frames
    Time m_currentTime;
    bool m_isPaused;
    // Total animation duration
    Time duration;
    Time passed_after_stop;

    std::shared_ptr<spdlog::logger> graphics_logger;

public:
    std::string name;

    explicit AnimatedSprite(std::string name, Time frameTime = seconds(0.2f), bool paused = false, bool looped = true, bool reversible = false);
    AnimatedSprite(std::string name, Texture* texture, IntRect frame0);

    void setAnimation(Animation* animation);
    // set time for which each frame lasts
    void setFrameTime(Time time);
    virtual void play();
    // Play with offset. Use <shift> and <frame_start> to manage offset
    virtual void play(std::size_t frame_start, Time shift=seconds(0));
    // set Animation and then play. You can play animation not from the beginning with <shift>
    virtual void play(Animation* animation, Time shift=seconds(0));
    // set Animation and then play. You can play animation not from the beginning with <shift> and <frame_start>
    virtual void play(Animation* animation, std::size_t frame_start, Time shift=seconds(0));
    virtual void pause();
    // pause and reset animation timer (revert to first frame)
    virtual void stop();
    // sets frame to stop after. If -1, sets to size - 1
    virtual void stop_after(int frame);

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
    virtual Vector2f getPosition() const;
    virtual void setPosition(const Vector2f &position);
    virtual void setScale(const Vector2f &factors);
    virtual void setScale(float factorX, float factorY);

    virtual bool isLooped() const;
    virtual bool isReversed() const;
    virtual bool isReversible() const;
    virtual bool isPlaying() const;
    virtual Time getFrameTime() const;
    virtual void setFrame(std::size_t newFrame, bool resetTime = true);
    virtual size_t getFrame() const;

    // whole animation duration
    Time get_duration() const;
    // time passed after animation reached end
    Time time_after_stop() const;

    virtual Time animation_remaining_time() const;
    virtual Time animation_remaining_time(size_t to_frame) const;
    // for VisualEffect inheritance
    virtual Time movement_remaining_time() const;

    // overriding Drawable methods
    virtual void update(Time deltaTime);
    virtual void redraw(RenderTarget& target, RenderStates states) const;
    virtual void draw(RenderTarget& target, RenderStates states) const override;

    // onclick action
    void onClick(bool pressed);
};

#endif // ANIMATEDSPRITE_INCLUDE
