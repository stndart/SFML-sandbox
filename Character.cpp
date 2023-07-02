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

Character::Character(string name, Texture &texture_default, IntRect frame0) :
moving(false), moving_enabled(false), animated(false),
facing_direction(0), moving_direction(0),
next_movement_direction(0), next_movement_shift(Vector2f(0, 0)),
name(name), switched_to_next_animation(false)
{
    base_sprite = new AnimatedSprite(name, texture_default, frame0);

    /// MAGIC CONSTANT!
    base_sprite->setFrameTime(seconds(0.01));
    //base_sprite->setFrameTime(seconds(0.04));
    no_return_point_time = seconds(0.1);

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

// flag if next animation is already need to be set up
bool Character::passedNoReturn()
{
//    std::cout << "No Return Check ";
    // if next movement is scheduled, we don't care of breakpoint is passed
    if (has_next_movement())
    {
//        std::cout << "HAS next movement ";
//        std::cout << get_next_movement_direction() << std::endl;
        return false;
    }

    Time breakpoint = max(base_sprite->animation_remaining_time(),
                          moving_sprite->movement_remaining_time());

//    if (breakpoint.asSeconds() >= no_return_point_time.asSeconds())
//        std::cout << "time has yet to come\n";
//    else
//        std::cout << "LETS GOOO\n";

    return breakpoint.asSeconds() < no_return_point_time.asSeconds();
}

// add animation to map by name
void Character::add_animation(string animation_name, Animation* p_animation)
{
    animations[animation_name] = p_animation;
}

// set current animation by name
void Character::set_animation(string animation_name, Time shift)
{
    std::cout << "Character::setting animation to " << animation_name << std::endl;
    base_sprite->play(*animations[animation_name], shift);
    animated = true;
    if (animation_name != idle_animation)
        base_sprite->setLooped(false);
    else
        base_sprite->setLooped(true);
}

// set current animation to idle with current direction
void Character::set_animation_to_idle(Time shift)
{
    string animation_name = get_idle_animation_s(get_current_direction());
    if (animations.count(animation_name) == 0)
        animation_name = idle_animation;

    std::cout << "Character::setting animation to " << animation_name << std::endl;
    base_sprite->play(*animations[animation_name], shift);
    animated = true;
    base_sprite->setLooped(true);
}

// clear animations queue and schedule next animation
void Character::set_next_animation(string animation_name)
{
    next_animations.clear();
    add_next_animation(animation_name);
}

// add next animation to the end of queue
void Character::add_next_animation(string animation_name)
{
    base_sprite->setLooped(false);
    next_animations.push_back(animation_name);
}

// flag if scheduled movement is present
bool Character::has_next_movement() const
{
    return next_movement_scheduled;
}

// scheduled movement direction
// need for smooth animation transitions
int Character::get_next_movement_direction() const
{
    return next_movement_direction;
}

void Character::cancel_next_movement()
{
//    std::cout << "Character::cancel next\n";
    next_movement_scheduled = false;
}

int Character::get_current_direction() const
{
    return moving_direction;
}

// schedule next movement with shift, direction (optional), animation name (default: <movement_%d>), duration (default: 2 seconds)
void Character::movement(Vector2f shift, int direction, string animation_name, Time duration)
{
//    std::cout << "Character::movement schedule dir " << direction << " and shift " << shift.x << " " << shift.y << std::endl;

    // if idle_animation, then finish it as soon as animation reaches end
    base_sprite->setLooped(false);

    // correct default arguments to valid

    // try to get animation by format <movement_%d>, where %d is direction
    // if no appropriate animation found, then <movement>
    // if no <movement> animation found, then skip animation (just smooth transition with default animation)
    if (animation_name.length() == 0)
    {
        animation_name = get_movement_animation_s(direction);
        // <movement_0> is default animation for movement
        if (animations.count(animation_name) == 0)
            animation_name = "movement_0";
        // idle animation is default animation
        if (animations.count(animation_name) == 0)
            animation_name = "";
    }

    // add movement to next;
    // here we assume movement won't be called more than once after point-of-no-return
    next_movement_scheduled = true;
    next_movement_shift = shift;
    next_movement_direction = direction;
    next_movement_animation_name = animation_name;
    next_movement_duration = duration;
}

// take movement and animation from next_animation and play
void Character::next_movement_start()
{
//    std::cout << "Character::next_movement_start with shift " << next_movement_shift.x << " " << next_movement_shift.y << std::endl;

    Vector2f shift = next_movement_shift;
    int direction = next_movement_direction;
    string animation_name = next_movement_animation_name;
    /// NOT USED (set to VE constructor)
    // Time duration = next_movement_duration;
    next_movement_scheduled = false;

    if (shift != Vector2f(0, 0))
    {
        // change current direction
        moving_direction = direction;

        // configure VE
        // get start and stop screen points (do not confuse with view points, which should fit in screen)
        Vector2f start = base_sprite->getPosition();
        Vector2f finish = start + shift;

        // offset animation with delay caused by flipping animations (e.g. idle_animation)
        Time offset = seconds(0);
        if (animated)
            offset = -base_sprite->animation_remaining_time();
        // offset animation with negative delay so that it would stitch together with last animation
        if (offset == seconds(0))
            offset = base_sprite->time_after_stop();

        // if current animation is complex (consists of queued animations)
        // further shift offset if animations queue is not empty
        for (auto anim : next_animations)
        {
            offset -= base_sprite->getFrameTime() * (float)animations[anim]->getSize();
        }
//        std::cout << "| Creating VisualEffect with offset " << offset.asSeconds() << std::endl;

        // we still can't put VE in VE
        if (moving_sprite != base_sprite)
            throw;

        // flip character sprite with <smooth transition from A to B> VisualEffect
        moving_sprite = new VisualEffect(base_sprite, offset, seconds(0.4), start, finish);
        // push back valid animation_name
        if (animation_name.length() > 0)
        {
//            std::cout << "pushing back " << animation_name << std::endl;
            add_next_animation(animation_name);
        }

        // start movement
        moving = true;
    }
    moving_sprite->play();
}

void Character::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    moving_sprite->setPosition(position);
}

void Character::setScale(const Vector2f &factors)
{
    Transformable::setScale(factors);
    moving_sprite->setScale(factors);
}

Vector2f Character::getPosition() const
{
    return moving_sprite->getPosition();
}

void Character::update(Time deltaTime)
{
    // if VE has reached the end, unwrap VE
    if (moving_sprite != base_sprite && moving_sprite->movement_remaining_time() <= seconds(0))
    {
//        std::cout << "Deleting VisualEffect\n";
        delete moving_sprite;
        moving_sprite = base_sprite;
        moving = false;
    }

    // if animation stopped, then play animation from queue
    if (!base_sprite->isPlaying() && next_animations.size() > 0)
    {
//        std::cout << "Character::update - pop next animation " << next_animations.front() << std::endl;
        set_animation(next_animations.front(), base_sprite->time_after_stop());
        next_animations.pop_front();
    }
//    std::cout << "Char::update moving: " << moving << " isplay: " << base_sprite->isPlaying() << std::endl;
    // if no VE and animation stopped, then invoke next_movement_start
    if (!moving && !base_sprite->isPlaying() && next_movement_scheduled)
    {
//        std::cout << "Character::update - new movement " << next_movement_animation_name << std::endl;
        next_movement_start();
    }
    // if there is still no animation, then set to idle_animation
    if (!base_sprite->isPlaying())
    {
//        std::cout << "Set idle looped because no play\n";
        set_animation_to_idle(base_sprite->time_after_stop());
        base_sprite->setLooped(true);
    }

    moving_sprite->update(deltaTime);
}

void Character::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(*moving_sprite);
}
