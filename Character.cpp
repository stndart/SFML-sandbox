#include "Character.h"

Character::Character()
{
    // NONE;
}

Character::Character(string name, Texture &texture_default, IntRect frame0) : name(name), animated(false), is_moving(false), facing_direction(0)
{
    base_sprite = new AnimatedSprite(name, texture_default, frame0);
    base_sprite->setFrameTime(seconds(0.05));
    sprite = base_sprite;
    idle_animation = "idle_animation_0";
    next_animation = {""};
    animations[idle_animation] = new Animation();
    int spritesheet_index = animations[idle_animation]->addSpriteSheet(texture_default);
    animations[idle_animation]->addFrame(frame0, spritesheet_index);
}

string get_idle_animation_s(int direction)
{
    string new_idle_anim = "idle_animation_";
    new_idle_anim += to_string(direction);
    return new_idle_anim;
}

string get_movement_animation_s(int direction)
{
    string new_move_anim = "movement_";
    new_move_anim += to_string(direction);
    return new_move_anim;
}

void Character::set_facing_direction(int new_direction)
{
    facing_direction = new_direction;
    if (animations.count(get_idle_animation_s(new_direction)) > 0)
        idle_animation = get_idle_animation_s(new_direction);
    else
        idle_animation = get_idle_animation_s(0);

    update(seconds(0));
}

int Character::get_facing_direction() const
{
    return facing_direction;
}

bool Character::isAnimated() const
{
    return animated;
}

bool Character::isMoving() const
{
    return is_moving;
}

void Character::add_animation(string animation_name, Animation* p_animation)
{
    animations[animation_name] = p_animation;
}

void Character::set_animation(string animation_name)
{
    sprite->play(*animations[animation_name]);
    if (animation_name != idle_animation)
        animated = true;
}

void Character::set_next_animation(string animation_name)
{
    if (animated)
    {
        next_animation = animation_name;
    }
    else
    {
        set_animation(animation_name);
    }
}

void Character::movement(Vector2f shift, int direction, string animation_name, Time duration)
{
    cout << "is moving " << is_moving << " animated " << animated << " playing " << sprite->isPlaying() << endl;
    if (is_moving)
        return;

    if (direction == -1)
    {
        if (abs(shift.x) > abs(shift.y))
        {
            if (shift.x > 0)
                direction = 0;
            else
                direction = 2;
        }
        else
        {
            if (shift.y > 0)
                direction = 3;
            else
                direction = 1;
        }
    }

    cout << "------move " << direction << " with shift " << shift.x << " " << shift.y << endl;

    if (animation_name.length() == 0)
    {
        animation_name = get_movement_animation_s(direction);
        if (animations.count(animation_name) == 0)
            animation_name = "movement";
        if (animations.count(animation_name) == 0)
            animation_name = "";
    }

    Vector2f start = base_sprite->getPosition();
    Vector2f finish = start + shift;

    sprite = new VisualEffect(base_sprite, seconds(0), seconds(0.2), start, finish);
    if (animation_name.length() > 0)
        sprite->setAnimation(*animations[animation_name]);

    is_moving = true;
    animated = true;
    sprite->play();
}

void Character::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    sprite->setPosition(position);
    // base_sprite->setPosition(position); // redundant
}

Vector2f Character::getPosition() const
{
    return sprite->getPosition();
}

void Character::update(Time deltaTime)
{
    //cout << "Character update is moving? " << is_moving << " VE? " << (sprite != base_sprite) << endl;
    if (!is_moving)
    {
        base_sprite->pause();
        if (sprite != base_sprite)
        {
            sprite->pause();
            cout << "DELETED\n";
            delete sprite;
            sprite = base_sprite;
        }
    }

    //cout << "Character animated? " << animated << " playing? " << sprite->isPlaying() << endl;
    if (animated)
    {
        if (!sprite->isPlaying())
        {
            is_moving = false;

            animated = false;
            if (next_animation != "")
            {
                set_animation(next_animation);
                next_animation = "";
            }
        }
    }

    sprite->update(deltaTime);
}

void Character::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(*sprite);
}
