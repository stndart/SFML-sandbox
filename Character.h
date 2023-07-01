#ifndef CHARACTER_INCLUDE
#define CHARACTER_INCLUDE

#include <SFML/System/Time.hpp>

#include <iostream>
#include <string>
#include <deque>
#include <map>

#include "Animation.h"
#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "extra_algorithms.h"

using namespace std;
using namespace sf;

class Character : public Drawable, public Transformable
{
    public:
        string name;

        AnimatedSprite* base_sprite;

        AnimatedSprite* moving_sprite;
        // default animation by name
        string idle_animation;
        // queue of scheduled animations by name
        deque<string> next_animations;
        // flag if current animation is finished
        // for external uses
        bool switched_to_next_animation;

        // set facing direction and change idle animation by direction (default: idle_animation_0)
        void set_facing_direction(int new_direction);
        int get_facing_direction() const;

        // flag if movement is active
        bool is_moving() const;
        // flag if smooth movement is enabled (teleports otherwise)
        bool is_moving_enabled() const;
        void set_moving_enabled(bool enabled);
        // flag if has valid animation (not static image)
        bool is_animated() const;

        Character();
        Character(string name, Texture& texture_default, IntRect frame0);
        Character(string name, map<string, Animation*> animations);

        // add animation to map by name
        void add_animation(string animation_name, Animation* p_animation);
        // set current animation by name
        void set_animation(string animation_name);
        // clear animations queue and schedule next animation
        void set_next_animation(string animation_name);
        // add next animation to the end of queue
        void add_next_animation(string animation_name);

        // flag if scheduled movement is present
        bool has_next_movement() const;
        // scheduled movement direction
        // need for smooth animation transitions
        int get_next_movement_direction() const;
        void cancel_next_movement();

        int get_current_direction() const;

        // invoke movement with shift, direction (optional), animation name (default: <movement_%d>), duration (default: 2 seconds)
        // if already moving, then schedule next movement
        void movement(Vector2f shift, int direction=-1, string animation_name="", Time duration=seconds(2));
        // if movement timer is up, then unwrap VisualEffect, transfer to next movement / terminate
        // note: while animations can be infinitely queued, movement supports only one scheduled move
        // note 2: Character supports only one level of VisualEffect wrap inside.
        // namely, Character creates only one VisualEffect wrap inside and utilizes only it.
        // It can be utilized with wrap inside: base_sprite can be VisualEffect (but no one can unwrap it then)
        void end_movement();

        // overriding Transformable methods
        virtual void setPosition(const Vector2f &position);
        virtual Vector2f getPosition() const;

        // overriding Drawable methods
        virtual void update(Time deltaTime = seconds(0));
        virtual void draw(RenderTarget& target, RenderStates states) const override;
    private:
        // flag if movement is active
        bool moving;
        // flag if smooth movement is enabled (teleports otherwise)
        bool moving_enabled;
        // flag if has valid animation (not static image)
        bool animated;
        int facing_direction; // standard 0 - right, 1 - down, 2 - left, 3 - up
        int moving_direction;

        // scheduled movement parameters
        int next_movement_direction;
        Vector2f next_movement_shift;
        string next_movement_animation_name;
        Time next_movement_duration;
        // if animation ends between frames, we save remaining time to transfer it to the next animation
        Time after_last_animation;

        map<string, Animation*> animations;
};

#endif
