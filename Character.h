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
    private:
        // flag if movement is active
        bool moving;
        // flag if smooth movement is enabled (teleports otherwise)
        bool moving_enabled;
        // flag if has valid animation (not static image)
        bool animated;
        int facing_direction; // standard 0 - right, 1 - down, 2 - left, 3 - up
        int moving_direction;
        bool next_movement_scheduled;

        // scheduled movement parameters
        int next_movement_direction;
        Vector2f next_movement_shift;
        string next_movement_animation_name;
        Time next_movement_duration;
        // if animation ends between frames, we save remaining time to transfer it to the next animation
        Time after_last_animation;
        Time no_return_point_time;

        map<string, Animation*> animations;

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
        // flag if next animation is already need to be set up
        bool passedNoReturn();

        Character();
        Character(string name, Texture& texture_default, IntRect frame0);
        Character(string name, map<string, Animation*> animations);

        // add animation to map by name
        void add_animation(string animation_name, Animation* p_animation);
        // set current animation by name with time shift
        void set_animation(string animation_name, Time shift = seconds(0));
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

        // schedule next movement with shift, direction (optional), animation name (default: <movement_%d>), duration (default: 2 seconds)
        void movement(Vector2f shift, int direction=-1, string animation_name="", Time duration=seconds(2));
        // take movement and animation from next_animation and play
        void next_movement_start();

        // overriding Transformable methods
        virtual void setPosition(const Vector2f &position);
        virtual Vector2f getPosition() const;
        virtual void setScale(const Vector2f &factors);

        // overriding Drawable methods
        virtual void update(Time deltaTime = seconds(0));
        virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif
