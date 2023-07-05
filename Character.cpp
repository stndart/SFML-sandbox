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
name(name)
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

    string anim = animation_name;
    if (animation_name.length() == 0)
    {
        if (shift != Vector2f(0, 0))
        {
            anim = get_movement_animation_s(direction);
            // <movement_0> is default animation for movement
            if (animations.count(anim) == 0)
                anim = "movement_0";
        }
        else
        {
            anim = get_idle_animation_s(direction);
        }
        // idle animation is default animation
        if (animations.count(anim) == 0)
            anim = "";
    }

    return anim;
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

// find last joint of transition between <current_animation> and <animation_name>
Joint Character::last_joint(string animation_name) const
{
    vector<string> transitions;
    // intersects set of animations AFTER <current_animation> and BEFORE <animation_name>
    const set<string> set_subsequent = animation_subsequent.at(current_animation);
    const set<string> set_previous = animation_previous.at(animation_name);
    // if target animation is available without any intermediate animations
    if (set_subsequent.contains(animation_name))
        transitions.push_back(animation_name);
    set_intersection(set_subsequent.begin(), set_subsequent.end(),
                     set_previous.begin(), set_previous.end(),
                     std::back_inserter(transitions));

    return animations.at(current_animation)->get_last_joint((int)base_sprite->getFrame(), transitions);
}

deque<Joint> Character::find_next_joint(string animation_name) const
{
    // queue of animations until min_frame
    deque<Joint> min_call_stack;
    int min_frame = -1;

    // we maintain deque sorted by number of start frame (assuming all animations have same frametime)
    deque<Joint> visited;
    deque<int> visited_frame;

    for (Joint j_base : animations.at(current_animation)->get_joints())
    {
        // We have starting frame: sprite->getFrame()
        if ((std::size_t)j_base.frame < base_sprite->getFrame())
            continue;

        visited.clear();
        visited_frame.clear();
        visited.push_back(j_base);
        visited_frame.push_back(j_base.frame - base_sprite->getFrame());

        // save for optimal call_stack
        deque<Joint> call_stack;
        call_stack.push_back(j_base);

        // now we perform bfs
        bool bfs_complete = false;
        Joint j;
        while(visited.size() > 0)
        {
            j = visited.front();
            visited.pop_front();

            string anim = j.anim_to;
            int frame_to = j.frame_to;

            int frame = visited_frame.front();
            visited_frame.pop_front();
            int duration;

            call_stack.push_back(j);

            for (Joint j_next : animations.at(anim)->get_joints())
            {
                // We have starting frame: j.frame_to
                if (j_next.frame < j.frame_to)
                    continue;

                // if we found the path to target animation then update min
                if (j_next.anim_to == animation_name)
                    if (min_frame == -1 || min_frame < frame)
                    {
                        min_frame = frame;
                        bfs_complete = true;

                        // refresh min call stack
                        min_call_stack = deque<Joint>(call_stack.begin(), call_stack.end());

                        break;
                    }

                // time from beginning of animation to joint
                duration = j_next.frame - frame_to;
                // now we find new place to insert joint
                std::size_t i = 0;
                for (; i < visited_frame.size(); ++i)
                    if (frame + duration < visited_frame[i])
                        break;

                visited.insert(next(visited.begin(), i), j_next);
                visited_frame.insert(next(visited_frame.begin(), i), frame + duration);
            }
            if (bfs_complete)
                break;

            call_stack.pop_back();
        }
    }

    return min_call_stack;
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
void Character::set_animation(string animation_name, Time offset, int frame_stop_after)
{
    std::cout << "Character::setting animation to " << animation_name << std::endl;

    current_animation = animation_name;

    base_sprite->play(*animations[animation_name], offset);
    base_sprite->stop_after(frame_stop_after);

    animated = true;
    if (animation_name != idle_animation)
        base_sprite->setLooped(false);
    else
        base_sprite->setLooped(true);
}

// set current animation to idle with current direction
void Character::set_animation_to_idle(Time offset)
{
    string animation_name = get_idle_animation_s(get_current_direction());
    if (animations.count(animation_name) == 0)
        animation_name = idle_animation;

    std::cout << "Character::setting animation to " << animation_name << std::endl;
    current_animation = animation_name;

    base_sprite->play(*animations[animation_name], offset);
    animated = true;
    base_sprite->setLooped(true);
}

// scheduled movement direction
// need for smooth animation transitions
int Character::get_next_movement_direction() const
{
    return next_animation_dir;
}

bool Character::order_completed()
{
    if (is_order_completed)
    {
        is_order_completed = false;
        return true;
    }
    return false;
}

int Character::get_current_direction() const
{
    return moving_direction;
}

string Character::get_current_animation() const
{
    return current_animation;
}

Vector2f Character::get_current_shift() const
{
    return moving_shift;
}

// schedules next movement. If planned movement is already late to cancel, plans after it
void Character::plan_movement(Vector2f shift, int direction, string animation_name, Time duration)
{
    // if idle_animation, then finish it as soon as animation reaches end
    base_sprite->setLooped(false);

    // try to get animation by format <movement_%d>, where %d is direction
    // if no appropriate animation found, then <movement>
    // if no <movement> animation found, then skip animation (just smooth transition with default animation)
    animation_name = get_animation_name_by_shift(shift, direction, animation_name);

    deque<Joint> next_joints = find_next_joint(animation_name);
    // if no path to target animation_name found, then break
    if (next_joints.size() == 0)
        return;
    // otherwise, construct queue
    next_animations.clear();
    std::size_t i = 0;
    Joint j;
    for (; i < next_joints.size() - 1; ++i)
    {
        j = next_joints[i];

        AnimMovement am;
        am.direction = direction;
        am.shift = Vector2f(0, 0);
        am.animation = j.anim_to;
        am.VE_allowed = false;
        if (next_joints[i + 1].anim_to == animation_name && next_joints[i + 1].frame_to > 1)
        {
            am.VE_allowed = true;
            am.has_VE = true;
            am.VE_start = base_sprite->getPosition(); /// is valid ?
            am.VE_shift = shift;
            am.VE_duration = duration;
        }
        am.j = next_joints[i];
        am.jnext = next_joints[i + 1];

        next_animations.push_back(am);
    }

    // last AnimMovement
    {
        AnimMovement am;
        am.direction = direction;
        am.shift = Vector2f(0, 0);
        am.animation = animation_name;
        am.VE_allowed = true;
        am.has_VE = false;
        if (!next_animations.rend()->has_VE)
        {
            am.has_VE = true;
            am.VE_start = base_sprite->getPosition(); /// is valid ?
            am.VE_shift = shift;
            am.VE_duration = duration;
        }
        am.j = *next_joints.rend();
        am.jnext = Joint({-1, "", 1});

        next_animations.push_back(am);
    }
}

// pops and plays an animation from AnimMovement deque (VEs as well)
void Character::next_movement_start()
{
    AnimMovement am = next_animations.front();
    next_animations.pop_front();
    set_animation(am.animation, -base_sprite->time_after_stop(), am.jnext.frame);

    if (am.has_VE)
    {
        Time offset = seconds(0);
        if (am.jnext.frame != -1)
        {
            offset += seconds(am.jnext.frame_to * base_sprite->getFrameTime().asSeconds());
            offset -= seconds((am.jnext.frame - am.j.frame_to) * base_sprite->getFrameTime().asSeconds());
        }
        moving_sprite = new VisualEffect(base_sprite, offset, am.VE_duration, am.VE_start, am.VE_start + am.VE_shift);
        moving_sprite->play();
        is_order_completed = true;

        moving_shift = am.VE_shift;
        moving = true;
    }
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
        std::cout << "Deleting VisualEffect\n";
        delete moving_sprite;
        moving_sprite = base_sprite;

        moving = false;
    }

    /// TO IMPLEMENT
}

void Character::update_old(Time deltaTime)
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
        std::cout << "Character::update - pop next animation " << next_animations.front().animation << std::endl;
        set_animation(next_animations.front().animation, base_sprite->time_after_stop(), next_animations.front().j.frame);
        next_animations.pop_front();
        //stop_after.pop_front();
    }
//    std::cout << "Char::update moving: " << moving << " isplay: " << base_sprite->isPlaying() << std::endl;
    // if no VE and animation stopped, then invoke next_movement_start
    if (!moving && !base_sprite->isPlaying())
    {
        std::cout << "Character::update - new movement " << next_animations.front().animation << std::endl;
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
