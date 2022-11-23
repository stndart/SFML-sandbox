#ifndef CHARACTER_INCLUDE
#define CHARACTER_INCLUDE

#include <SFML/System/Time.hpp>

#include <iostream>
#include <string>
#include <map>

#include "Animation.h"
#include "AnimatedSprite.h"
#include "VisualEffect.h"

using namespace std;
using namespace sf;

class Character : public Drawable, public Transformable
{
    public:
        string name;
        AnimatedSprite* base_sprite;
        AnimatedSprite* sprite;
        string idle_animation;
        string next_animation;
        void set_facing_direction(int new_direction);
        int get_facing_direction() const;
        bool isAnimated() const;
        bool isMoving() const;

        Character();
        Character(string name, Texture& texture_default, IntRect frame0);
        Character(string name, map<string, Animation*> animations);

        void add_animation(string animation_name, Animation* p_animation);
        void set_animation(string animation_name);
        void set_next_animation(string animation_name);

        // direction = -1 if unspecified, then automatic
        void movement(Vector2f shift, int direction=-1, string animation_name="", Time duration=seconds(2));
        virtual void setPosition(const Vector2f &position);
        virtual Vector2f getPosition() const;

        virtual void update(Time deltaTime);
        virtual void draw(RenderTarget& target, RenderStates states) const override;
    private:
        bool animated;
        bool is_moving;
        int facing_direction; // standard 0 - right, 1 - down, 2 - left, 3 - up
        map<string, Animation*> animations;
};

#endif
