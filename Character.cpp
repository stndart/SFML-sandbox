#include "Character.h"

// get default name of idle animation with direction
string get_idle_animation_s(int direction)
{
    string new_idle_anim = "idle_animation_";
    new_idle_anim += to_string(direction);
    return new_idle_anim;
}

// get default name of movement animation with direction
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

Character::Character(string name, Texture &texture_default, IntRect frame0) : name(name), switched_to_next_animation(false),
moving(false), moving_enabled(false), animated(false),
facing_direction(0), moving_direction(0),
next_movement_direction(0), next_movement_shift(Vector2f(0, 0))
{
    base_sprite = new AnimatedSprite(name, texture_default, frame0);
    base_sprite->setFrameTime(seconds(0.05));
    moving_sprite = base_sprite;
    idle_animation = "idle_animation_0";
    next_animations = deque<string>();
    animations[idle_animation] = new Animation();
    int spritesheet_index = animations[idle_animation]->addSpriteSheet(texture_default);
    animations[idle_animation]->addFrame(frame0, spritesheet_index);
    next_movement_duration = seconds(0);
    after_last_animation = seconds(0);
}

// set facing direction and change idle animation by direction (default: idle_animation_0)
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

// flag if movement is active
bool Character::is_moving() const
{
    return moving;
}

// flag if smooth movement is enabled (teleports otherwise)
bool Character::is_moving_enabled() const
{
    return moving_enabled;
}

void Character::set_moving_enabled(bool enabled)
{
    moving_enabled = enabled;
}

// flag if has valid animation (not static image)
bool Character::is_animated() const
{
    return animated;
}

// add animation to map by name
void Character::add_animation(string animation_name, Animation* p_animation)
{
    animations[animation_name] = p_animation;
}

// set current animation by name
void Character::set_animation(string animation_name)
{
    base_sprite->play(*animations[animation_name]);
    if (animation_name != idle_animation)
        animated = true;
}

// clear animations queue and schedule next animation
void Character::set_next_animation(string animation_name)
{
    while (next_animations.size() > 0)
        next_animations.pop_front();
    next_animations.push_back(animation_name);
}

// add next animation to the end of queue
void Character::add_next_animation(string animation_name)
{
    next_animations.push_back(animation_name);
}

// flag if scheduled movement is present
bool Character::has_next_movement() const
{
    return next_movement_shift != Vector2f(0, 0);
}

// scheduled movement direction
// need for smooth animation transitions
int Character::get_next_movement_direction() const
{
    return next_movement_direction;
}

void Character::cancel_next_movement()
{
    next_movement_shift = Vector2f(0, 0);
}

int Character::get_current_direction() const
{
    return moving_direction;
}

// invoke movement with shift, direction (optional), animation name (default: <movement_%d>), duration (default: 2 seconds)
// if already moving, then schedule next movement
void Character::movement(Vector2f shift, int direction, string animation_name, Time duration)
{
    // delay this call until current movement is completed
    if (is_moving())
    {
        next_movement_shift = shift;
        next_movement_direction = direction;
        next_movement_animation_name = animation_name;
        next_movement_duration = duration;
        return;
    }

    // if direction is unspecified than get it from shift vector
    if (direction == -1)
        direction = direction_from_shift(shift);

    // if smooth moving enabled
    if (is_moving_enabled())
    {
        // try to get animation by format <movement_%d>, where %d is direction
        // if no appropriate animation found, then <movement>
        // if no <movement> animation found, then skip animation (just smooth transition with default animation)
        if (animation_name.length() == 0)
        {
            animation_name = get_movement_animation_s(direction);
            if (animations.count(animation_name) == 0)
                animation_name = "movement";
            if (animations.count(animation_name) == 0)
                animation_name = "";
        }
        moving_direction = direction;

        // get start and stop screen points (do not confuse with view points, which should fit in screen)
        Vector2f start = base_sprite->getPosition();
        Vector2f finish = start + shift;

        // offset animation with delay caused by flipping animations
        // sets only if current animation is scheduled after another animation
        Time offset = seconds(0);
        if (animated)
            offset = -base_sprite->animation_remaining_time();
        if (offset == seconds(0))
            offset = after_last_animation;
        // further shift offset if animations queue is not empty
        for (auto anim : next_animations)
        {
            offset -= base_sprite->getFrameTime() * (float)animations[anim]->getSize();
        }

        // flip character sprite with <smooth transition from A to B> VisualEffect
        moving_sprite = new VisualEffect(base_sprite, offset, seconds(0.2), start, finish);
        // push back valid animation_name
        if (animation_name.length() > 0)
            add_next_animation(animation_name);

        // start movement
        moving = true;
        moving_sprite->play();
    }
    // teleport sprite otherwise
    else
    {
        moving_sprite->move(shift);
    }
}

// if movement timer is up, then unwrap VisualEffect, transfer to next movement / terminate
// note: while animations can be infinitely queued, movement supports only one scheduled move
// note 2: Character supports only one level of VisualEffect wrap inside.
// namely, Character creates only one VisualEffect wrap inside and utilizes only it.
// It can be utilized with wrap inside: base_sprite can be VisualEffect (but no one can unwrap it then)
void Character::end_movement()
{
    if (!is_moving())
        throw;

    after_last_animation = moving_sprite->time_after_stop();

    delete moving_sprite;
    moving_sprite = base_sprite;
    moving = false;
    moving_direction = -1;

    if (next_movement_shift != Vector2f(0, 0))
    {
        switched_to_next_animation = true;
        //cout << "Scheduled direction became playing " << next_movement_direction << endl;
        movement(next_movement_shift, next_movement_direction, next_movement_animation_name, next_movement_duration);
        cancel_next_movement();
    }
    else
    {
        after_last_animation = seconds(0);
    }
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
