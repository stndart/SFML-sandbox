#include "Character.h"

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

Character::Character()
{
    // NONE;
}

Character::Character(string name, Texture &texture_default, IntRect frame0) : name(name), moving(false), moving_enabled(false), animated(false), facing_direction(0)
{
    base_sprite = new AnimatedSprite(name, texture_default, frame0);
    base_sprite->setFrameTime(seconds(0.05));
    moving_sprite = base_sprite;
    idle_animation = "idle_animation_0";
    next_animations = deque<string>();
    animations[idle_animation] = new Animation();
    int spritesheet_index = animations[idle_animation]->addSpriteSheet(texture_default);
    animations[idle_animation]->addFrame(frame0, spritesheet_index);
    after_last_animation = seconds(0);
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

bool Character::is_moving() const
{
    return moving;
}

bool Character::is_moving_enabled() const
{
    return moving_enabled;
}

void Character::set_moving_enabled(bool enabled)
{
    moving_enabled = enabled;
}

bool Character::is_animated() const
{
    return animated;
}

void Character::add_animation(string animation_name, Animation* p_animation)
{
    animations[animation_name] = p_animation;
}

void Character::set_animation(string animation_name)
{
    base_sprite->play(*animations[animation_name]);
    if (animation_name != idle_animation)
        animated = true;
}

void Character::set_next_animation(string animation_name)
{
    while (next_animations.size() > 0)
        next_animations.pop_front();
    next_animations.push_back(animation_name);
}

void Character::add_next_animation(string animation_name)
{
    next_animations.push_back(animation_name);
}

void Character::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    moving_sprite->setPosition(position);
    // base_sprite->setPosition(position); // redundant
}

Vector2f Character::getPosition() const
{
    return moving_sprite->getPosition();
}

void Character::cancel_next_movement()
{
    next_movement_shift = Vector2f(0, 0);
}

void Character::movement(Vector2f shift, int direction, string animation_name, Time duration)
{
    cout << "is moving " << is_moving() << " animated " << is_animated() << " playing " << moving_sprite->isPlaying() << endl;
    if (is_moving())
    {
        next_movement_shift = shift;
        next_movement_direction = direction;
        next_movement_animation_name = animation_name;
        next_movement_duration = duration;
    }

    if (direction == -1)
        direction = direction_from_shift(shift);

    //cout << "------move " << direction << " with shift " << shift.x << " " << shift.y << endl;

    if (is_moving_enabled())
    {
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

        Time offset = seconds(0);
        if (animated)
            offset = -base_sprite->animation_remaining_time();
        if (offset == seconds(0))
            offset = after_last_animation;
        for (auto anim : next_animations)
        {
            offset -= base_sprite->getFrameTime() * (float)animations[anim]->getSize();
        }
        moving_sprite = new VisualEffect(base_sprite, offset, seconds(0.2), start, finish);
        if (animation_name.length() > 0)
            add_next_animation(animation_name);

        moving = true;
        //cout << "new sprite offset " << offset.asMilliseconds() << endl;
        moving_sprite->play();
    }
    else
    {
        moving_sprite->move(shift);
    }
}

void Character::end_movement()
{
    if (!is_moving())
        throw;

    after_last_animation = moving_sprite->time_after_stop();

    delete moving_sprite;
    moving_sprite = base_sprite;
    moving = false;

    if (next_movement_shift != Vector2f(0, 0))
    {
        movement(next_movement_shift, next_movement_direction, next_movement_animation_name, next_movement_duration);
        cancel_next_movement();
    }
}

void Character::update(Time deltaTime)
{
    if (!animated)
        if (next_animations.size() > 0)
        {
            set_animation(next_animations[0]);
            next_animations.pop_front();
        }

    if (is_moving())
    {
        Time remain_mov = moving_sprite->movement_remaining_time();
        //cout << "ending movement " << remain_mov.asMilliseconds() << endl;
        if (remain_mov <= seconds(0))
            end_movement();
    }

    moving_sprite->update(deltaTime);
}

void Character::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(*moving_sprite);
}
