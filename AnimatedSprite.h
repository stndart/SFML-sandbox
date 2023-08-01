#ifndef ANIMATEDSPRITE_INCLUDE
#define ANIMATEDSPRITE_INCLUDE

#include <iostream>
#include <string>
#include <memory>

#include "Animation.h"
#include "extra_algorithms.h"

#include <spdlog/spdlog.h>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

using namespace sf;

class ResourceLoader;

// AnimatedSprite (referred as ASprite hereafter) is a drawable sprite
// ASprite has a Shape (rectangle, circle or polygonal), that can be filled with a color with alpha-channel and/or with texture.
// Color blending with underlying drawable objects by default works with alpha-channel (BlendAlpha), but works with any other BlendMode as well (no blending, adding/multiplying colors)
// Besides, ASprite can be set with dynamically switchable texture (i.e. Animation), in this case methods <play>, <stop>, <pause>, <stop_after> control playback of switching textures (frames).
// Animation can be looped, reversed or straight-forward
// Transformable methods are overridden, so they handle Shape and Animation correctly

class AnimatedSprite : public Drawable, public Transformable
{
private:
    // coordinates of ASprite
    Vector2f m_position;

    // pointer to spritesheet
    std::shared_ptr<Animation> m_animation;
    // one frame time length
    Time m_frameTime = seconds(0);
    // current frame index
    std::size_t m_currentFrame = 0;
    // index of frame to stop after (by default -1, i.e. don't stop)
    int frame_stop_after = -1;

    bool m_isLooped = false;
    bool m_isReversed = false;
    bool m_isReversible = false;

    // shape over which texture (or color) may be put
    std::unique_ptr<Shape> parent_shape;
    // blend mode of this sprite
    sf::BlendMode sprite_blend_mode;

    // returns false if <newframe> is after <stop_after> or out of bounds
    // in other words, returns true, if switching to <newframe> is provided by playback
    bool is_frame_valid(int newframe);

protected:
    // Inner time counter. Used to switch between frames
    Time m_currentTime = seconds(0);
    // equals !isPlaying
    bool m_isPaused = true;
    // Total animation duration
    Time duration;
    // Total time passed since pause
    Time passed_after_stop = seconds(0);

    // Smart pointers to logger handlers
    std::shared_ptr<spdlog::logger> loading_logger, graphics_logger;

public:
    std::string name;
    // needed for external purposes (sorting sprites by z-index)
    int z_index;

    // creates ASrite with given shape and position. Shape can have texture with texcoords set
    // shape is unchangeable after initialization
    // accepts origin as well. Default it top-left
    AnimatedSprite(std::string name, std::unique_ptr<Shape> shape, Vector2f pos, Vector2f origin = Vector2f(0, 0), int z_ind = 0, sf::BlendMode blend_mode = sf::BlendAlpha);
    // creates ASprite with rectangular shape of <posrect> size and position and <texrect> texture coordinates
    AnimatedSprite(std::string name, std::shared_ptr<Texture> texture, IntRect texrect, FloatRect posrect, Vector2f origin = Vector2f(0, 0), int z_ind = 0, sf::BlendMode blend_mode = sf::BlendAlpha);
    // creates ASprite with given animation and parameters (frametime, paused, looped, reversible)
    AnimatedSprite(std::string name, std::shared_ptr<Animation> animation, Time frameTime, FloatRect posrect, Vector2f origin = Vector2f(0, 0),
                   bool looped = true, bool reversible = false, int z_ind = 0, sf::BlendMode blend_mode = sf::BlendAlpha);
    // creates ASprite with animation/texture name at position and size. Texture loads by <animation_name> if there is no such animation, but there is texture
    AnimatedSprite(
        std::string name, std::shared_ptr<ResourceLoader> resload,
        std::string animation_name, Time frameTime,
        FloatRect posrect, Vector2f origin = Vector2f(0, 0),
        int z_ind = 0);

    // Animation parameters setters/getters
    // for external uses after VisualEffect inheritance
    virtual std::shared_ptr<Animation> getAnimation();
    // set animation spritesheet
    virtual void setAnimation(std::shared_ptr<Animation> animation);
    // set time for which each frame lasts
    void setFrameTime(Time time);
    // resume playback from current position. No frame resetting
    virtual void play();
    // Play with offset. Use <shift> and <frame_start> to manage offset
    virtual void play(std::size_t frame_start, Time shift=seconds(0));
    // set Animation and then play. You can play animation not from the beginning with <shift>
    virtual void play(std::shared_ptr<Animation> animation, Time shift=seconds(0));
    // set Animation and then play. You can play animation not from the beginning with <shift> and <frame_start>
    virtual void play(std::shared_ptr<Animation> animation, std::size_t frame_start, Time shift=seconds(0));
    // just pause. No frame resetting
    virtual void pause();
    // pause and reset frame and animation timer (revert to first frame)
    virtual void stop();
    // sets frame to stop after. If -1, sets to last frame
    virtual void stop_after(int frame);

    // replays animation from the beginning after the end is reached
    virtual void setLooped(bool looped);
    // is animation played backwards
    virtual void setReversed(bool reversed);
    // animation extends after its end with itself everted backwards.
    // m_isLooped in this case indicates if animation sways once or multiple times
    virtual void setReversible(bool reversible);

    virtual bool isLooped() const;
    virtual bool isReversed() const;
    virtual bool isReversible() const;
    virtual bool isPlaying() const;
    virtual Time getFrameTime() const;
    virtual void setFrame(std::size_t newFrame, bool resetTime = true);
    virtual size_t getFrame() const;

    // paints shape with color. by default shape is transparent
    virtual void setColor(const Color& color);

    virtual Time animation_remaining_time() const;
    virtual Time animation_remaining_time(size_t to_frame) const;
    // for VisualEffect inheritance
    virtual Time movement_remaining_time() const;
    // time passed after animation reached end
    Time time_after_stop() const;

    // overriding Transformable methods
    virtual void move(const Vector2f &shift);
    virtual void rotate(float angle);
    virtual void scale(const Vector2f &factor);
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;
    virtual const Vector2f& getPosition() const;
    virtual float getRotation() const;
    virtual const Vector2f& getScale() const;
    virtual void setPosition(const Vector2f &position);
    virtual void setPosition(float x, float y);
    virtual void setScale(const Vector2f &factors);
    virtual void setScale(float x, float y);
    virtual void setOrigin(const Vector2f &origin);
    virtual void setOrigin(float x, float y);

    // overriding Drawable methods
    virtual void update(Time deltaTime);
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // ANIMATEDSPRITE_INCLUDE
