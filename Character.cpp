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
facing_direction(0), moving_direction(0), next_movement_scheduled(false),
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

//    std::cout << "Character::Character constructing empty idle_animation\n";
    animations[idle_animation] = new Animation();
    int spritesheet_index = animations[idle_animation]->addSpriteSheet(texture_default);
    animations[idle_animation]->addFrame(frame0, spritesheet_index);

    next_movement_duration = seconds(0);
    after_last_animation = seconds(0);
//    std::cout << "Character::Character out\n";
}

string Character::get_animation_name_by_shift(Vector2f shift, int direction, string animation_name) const
{
    if (direction == -1)
    {
        if (shift != Vector2f(0, 0))
            direction = direction_from_shift(shift);
        else
            direction = facing_direction;
    }

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

    return animation_name;
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
bool Character::passedNoReturn() const
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

// find last joint of transition between <current_animation> and <animation_name>
Joint Character::last_joint(string animation_name) const
{
    vector<string> transitions;
    // intersects set of animations AFTER <current_animation> and BEFORE <animation_name>
    set_intersection(animation_subsequent.at(current_animation).begin(), animation_subsequent.at(current_animation).end(),
                     animation_previous.at(animation_name).begin(), animation_previous.at(animation_name).end(),
                     std::back_inserter(transitions));

    return animations.at(current_animation)->get_last_joint((int)base_sprite->getFrame(), transitions);
}

/// NOTE: it searches for next joint in current animation, not the last in <next_animations> (including current if empty)
Joint Character::next_available_joint(string animation_name) const
{
    // to find next available joint
    vector<string> transitions;
    // intersects set of animations AFTER <current_animation> and BEFORE <animation_name>
    const set<string> set_subsequent = animation_subsequent.at(current_animation);
    const set<string> set_previous = animation_previous.at(animation_name);
    set_intersection(set_subsequent.begin(), set_subsequent.end(),
                     set_previous.begin(), set_previous.end(),
                     std::back_inserter(transitions));
    // if target animation is available without any intermediate animations
    if (set_subsequent.contains(animation_name))
        transitions.push_back(animation_name);

    std::cout << "found transitions from " << current_animation << " to " << animation_name << "";
    for (std::size_t i = 0; i < transitions.size(); ++i)
        std::cout << ", " << transitions[i];
    std::cout << std::endl;

    Joint j = animations.at(current_animation)->get_next_joint((int)base_sprite->getFrame(), transitions);
    while (base_sprite->animation_remaining_time(j.frame).asSeconds() < no_return_point_time.asSeconds())
    {
        // find next joint after current
        Joint temp_j = animations.at(current_animation)->get_next_joint(j.frame + 1, transitions);
        // if reached end
        if (temp_j.frame == -1)
            break;
        // if we found available joint, then update
        j = temp_j;
    }
    return j;
}

// is it possible to change scheduled animation
bool Character::canReSchedule(Vector2f shift, int direction, string animation_name) const
{
    animation_name = get_animation_name_by_shift(shift, direction, animation_name);

    // find last joint of transition to check if it is available
    Joint j = last_joint(animation_name);
    // if no joint until end of animation then impossible
    if (j.frame == -1)
        return false;
    // Time untill last: movement ends or last animation joint
    Time breakpoint = max(base_sprite->animation_remaining_time(j.frame),
                          moving_sprite->movement_remaining_time());

    return breakpoint.asSeconds() < no_return_point_time.asSeconds();
}

// add animation to map by name
void Character::add_animation(string animation_name, Animation* p_animation)
{
    animations[animation_name] = p_animation;
    for (Joint j : p_animation->get_joints())
    {
//        std::cout << "Character: adding pair of anim: " << animation_name << " -> " << j.anim_to << std::endl;
        // <animation_name> can be followed by <j.anim_to> and vice versa
        animation_subsequent[animation_name].insert(j.anim_to);
        animation_previous[j.anim_to].insert(animation_name);
    }
}

// set current animation by name
void Character::set_animation(string animation_name, Time shift, int frame_stop_after)
{
    std::cout << "Character::setting animation to " << animation_name << std::endl;

//    std::cout << "check: subsequent and preciding animations:\n";
//    std::cout << "preciding: ";
//    for (string p_anim : animation_previous[animation_name])
//        std::cout << ", " << p_anim;
//    std::cout << "\nsubsequent: ";
//    for (string p_anim : animation_subsequent[animation_name])
//        std::cout << ", " << p_anim;
//    std::cout << "\ncheck out\n";

    current_animation = animation_name;

    base_sprite->play(*animations[animation_name], shift);
    // default argument
    if (frame_stop_after != -1)
        base_sprite->stop_after(frame_stop_after);

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
    current_animation = animation_name;

    base_sprite->play(*animations[animation_name], shift);
    animated = true;
    base_sprite->setLooped(true);
}

// clear animations queue and schedule next animation
void Character::set_next_animation(string animation_name, int frame_stop_after)
{
    next_animations.clear();
    stop_after.clear();
    add_next_animation(animation_name, frame_stop_after);
}

// add next animation to the end of queue
void Character::add_next_animation(string animation_name, int frame_stop_after)
{
    base_sprite->setLooped(false);
    next_animations.push_back(animation_name);
    stop_after.push_back(frame_stop_after);
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

string Character::get_current_animation() const
{
    return current_animation;
}

// schedule next movement with shift, direction (optional), animation name (default: <movement_%d>), duration (default: 2 seconds)
void Character::movement(Vector2f shift, int direction, string animation_name, Time duration)
{
    std::cout << "Character::movement schedule dir " << direction << " and shift " << shift.x << " " << shift.y << std::endl;

    // if idle_animation, then finish it as soon as animation reaches end
    base_sprite->setLooped(false);

    // correct default arguments to valid

    // try to get animation by format <movement_%d>, where %d is direction
    // if no appropriate animation found, then <movement>
    // if no <movement> animation found, then skip animation (just smooth transition with default animation)
    animation_name = get_animation_name_by_shift(shift, direction, animation_name);
    std::cout << "Character::movement: found anim name " << animation_name << std::endl;

    // find next joint that is minimum of no_return_point_time before it
    Joint j = next_available_joint(animation_name);
    std::cout << "Character::movement: found next joint with frame: " << j.frame << std::endl;
    base_sprite->stop_after(j.frame);
    std::cout << "Character::movement: and set current animation to stop after joint" << std::endl;

    // add movement to next;
    // here we assume movement won't be called more than once after point-of-no-return
    next_movement_scheduled = true;
    next_movement_shift = shift;
    next_movement_direction = direction;
    next_movement_animation_name = animation_name;
    next_movement_joint = j;
    next_movement_duration = duration;
}

// take movement and animation from next_animation and play
void Character::next_movement_start()
{
    std::cout << "Character::next_movement_start with shift " << next_movement_shift.x << " " << next_movement_shift.y << std::endl;

    Vector2f shift = next_movement_shift;
    int direction = next_movement_direction;
    string animation_name = next_movement_animation_name;
    Joint j = next_movement_joint;
    /// NOT USED (set to VE constructor)
    // Time duration = next_movement_duration;
    next_movement_scheduled = false;

    // change current direction
    moving_direction = direction;

    // push new animations in queue
    // firstly, push transition (with stop_after frame)
    add_next_animation(j.anim_to, animations[j.anim_to]->get_next_joint(j.frame_to, animation_name).frame);

    // offset animation with negative delay so that it would stitch together with last animation
    Time offset = seconds(0);
    if (offset == seconds(0))
        offset = base_sprite->time_after_stop();

    // if current animation is complex (consists of queued animations)
    // further shift offset if animations queue is not empty
    for (std::size_t i = 0; i < next_animations.size(); ++i)
    {
        string anim = next_animations[i];
        int frame_stop_after = stop_after[i];

        // count next_anim size. If stop_after is not default (-1), take into account
        float next_anim_size = animations[anim]->getSize();
        if (frame_stop_after != -1)
            next_anim_size = frame_stop_after;

        offset -= base_sprite->getFrameTime() * next_anim_size;
    }

    // push back valid animation_name
    if (animation_name.length() > 0)
    {
        // std::cout << "pushing back " << animation_name << std::endl;
        add_next_animation(animation_name);
    }

    // if we plan to move -> construct VE
    if (shift != Vector2f(0, 0))
    {
        // configure VE
        // get start and stop screen points (do not confuse with view points, which should fit in screen)
        Vector2f start = base_sprite->getPosition();
        Vector2f finish = start + shift;
        // std::cout << "| Creating VisualEffect with offset " << offset.asSeconds() << std::endl;

        // we still can't put VE in VE
        if (moving_sprite != base_sprite)
            throw;

        // flip character sprite with <smooth transition from A to B> VisualEffect
        moving_sprite = new VisualEffect(base_sprite, offset, seconds(0.4), start, finish);

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
//    std::cout << "Character::update\n";
    // if VE has reached the end, unwrap VE
    if (moving_sprite != base_sprite && moving_sprite->movement_remaining_time() <= seconds(0))
    {
        std::cout << "Deleting VisualEffect\n";
        delete moving_sprite;
        moving_sprite = base_sprite;
        moving = false;
    }

    // if animation stopped, then play animation from queue
    if (!base_sprite->isPlaying() && next_animations.size() > 0)
    {
        std::cout << "Character::update - pop next animation " << next_animations.front() << std::endl;
        set_animation(next_animations.front(), base_sprite->time_after_stop(), stop_after.front());
        next_animations.pop_front();
        stop_after.pop_front();
    }
//    std::cout << "Char::update moving: " << moving << " isplay: " << base_sprite->isPlaying() << std::endl;
    // if no VE and animation stopped, then invoke next_movement_start
    if (!moving && !base_sprite->isPlaying() && next_movement_scheduled)
    {
        std::cout << "Character::update - new movement " << next_movement_animation_name << std::endl;
        next_movement_start();
    }
    // if there is still no animation, then set to idle_animation
    if (!base_sprite->isPlaying())
    {
        std::cout << "Set idle looped because no play\n";
        set_animation_to_idle(base_sprite->time_after_stop());
        base_sprite->setLooped(true);
    }

    moving_sprite->update(deltaTime);
}

void Character::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(*moving_sprite);
}
