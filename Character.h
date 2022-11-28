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
        string idle_animation;
        deque<string> next_animations;
        bool is_moving() const;
        bool is_moving_enabled() const;
        void set_moving_enabled(bool enabled);
        bool is_animated() const;

        void set_facing_direction(int new_direction);
        int get_facing_direction() const;

        Character();
        Character(string name, Texture& texture_default, IntRect frame0);
        Character(string name, map<string, Animation*> animations);

        void add_animation(string animation_name, Animation* p_animation);
        void set_animation(string animation_name);
        void set_next_animation(string animation_name);
        void add_next_animation(string animation_name);

        bool has_next_movement() const;
        int get_next_movement_direction() const;
        void cancel_next_movement();
        bool switched_to_next_animation;

        int get_current_direction() const;

        // direction = -1 if unspecified, then automatic
        void movement(Vector2f shift, int direction=-1, string animation_name="", Time duration=seconds(2));
        void end_movement();
        virtual void setPosition(const Vector2f &position);
        virtual Vector2f getPosition() const;

        virtual void update(Time deltaTime = seconds(0));
        virtual void draw(RenderTarget& target, RenderStates states) const override;
    private:
        bool moving;
        bool moving_enabled;
        bool animated;
        int facing_direction; // standard 0 - right, 1 - down, 2 - left, 3 - up
        int moving_direction;

        int next_movement_direction;
        Vector2f next_movement_shift;
        string next_movement_animation_name;
        Time next_movement_duration;
        Time after_last_animation;

        map<string, Animation*> animations;
};

#endif
