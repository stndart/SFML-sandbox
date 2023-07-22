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

Character::Character(string name, Texture *texture_default, IntRect frame0) :
moving(false), moving_enabled(true), animated(false), is_order_completed(false), ignore_joints(false),
current_animation(""), facing_direction(0), moving_direction(0), moving_shift(Vector2f(0, 0)),
movement_started(false), next_movement_planned(false), next_animation_dir(-1),
name(name)
{
    // Reaching out to global "map_events" logger and "graphics" logger by names
    map_events_logger = spdlog::get("map_events");
    graphics_logger = spdlog::get("graphics");

    base_sprite = new AnimatedSprite(name, texture_default, frame0);

    /// MAGIC CONSTANT!
    base_sprite->setFrameTime(seconds(0.01));

    moving_sprite = base_sprite;
    set_facing_direction(0);

    animations[idle_animation] = new Animation();
    int spritesheet_index = animations[idle_animation]->addSpriteSheet(texture_default);
    animations[idle_animation]->addFrame(frame0, spritesheet_index);
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
    graphics_logger->trace("Get animation name with direction {} while facing {}", direction, facing_direction );

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
            // <idle_animation_0> is default animation for movement
            if (animations.count(anim) == 0)
                anim = "idle_animation_0";
        }
        // idle animation is default animation
        if (animations.count(anim) == 0)
            anim = "";
    }
    graphics_logger->trace("Got animation name: {}", anim);

    return anim;
}

// set facing direction and change idle animation by direction (default: idle_animation_0)
void Character::set_facing_direction(int new_direction)
{
    graphics_logger->debug("Character::set facing direction {}", new_direction);

    facing_direction = new_direction;
    if (animations.count(get_idle_animation_s(new_direction)) > 0)
        idle_animation = get_idle_animation_s(new_direction);
    else
        idle_animation = get_idle_animation_s(0);

    /// WHY? (temporarily switched off, since i don't know why it is here)
    //update(seconds(0));
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

// ignoring joints flag
bool Character::is_ignore_joints() const
{
    return ignore_joints;
}

void Character::set_ignore_joints(bool ignore)
{
    ignore_joints = ignore;
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

struct ParentJoint
{
    string anim;
    int frame;
    Joint j;
};

deque<Joint> Character::find_next_joint(string animation_name) const
{
    graphics_logger->debug("Character::find_next_joint from \"{}\":{} to \"{}\"", current_animation, base_sprite->getFrame(), animation_name);

    // queue of animations until min_frame
    deque<Joint> min_call_stack;
    Joint j;

    // if no current animation, then transition immediately
    if (current_animation == "")
    {
        j = Joint{0, animation_name, 0};
        min_call_stack.push_back(j);
        return min_call_stack;
    }
    // if any joint is OK, then return the very next one
    if (animation_name == "")
    {
        j = animations.at(current_animation)->get_next_joint(base_sprite->getFrame());
        graphics_logger->trace("returning joint [frame: {}, to frame: {}, to anim {}]", j.frame, j.frame_to, j.anim_to);

        min_call_stack.push_back(j);
        return min_call_stack;
    }

    // we maintain deque sorted by number of start frame (assuming all animations have same frametime)
    deque<Joint> visited;
    // absolute start frame of Joint in vertice
    deque<int> visited_frame;
    // map of parents (to restore path in bfs)
    map<pair<string, int>, ParentJoint> visited_parent;

    // map of minimum frame of visited animation
    // if animation is already visited, then pop from bfs
    map<string, int> visited_fromframe;

    visited.push_back(Joint{0, current_animation, (int)base_sprite->getFrame()});
    visited_frame.push_back((int)base_sprite->getFrame());
    visited_parent[make_pair(current_animation, -1)] = ParentJoint{"root", 0, visited.back()};
    // don't update min_frame_to here, bc current animation doesn't count
    graphics_logger->trace("+ adding start for <0, {}:>", current_animation, (int)base_sprite->getFrame());

    // now we perform bfs
    int frame;
    string anim;
    ParentJoint parent;
    // we can't end bfs just checking current animation
    bool first_cycle = true;
    while(visited.size() > 0)
    {
        j = visited.front();
        visited.pop_front();

        anim = j.anim_to;
        int frame_to = j.frame_to;

        frame = visited_frame.front();
        visited_frame.pop_front();
        int duration;

        graphics_logger->trace("bfs: current anim: {}, frame: {}", anim, frame);

        // if we found the path to target animation then update min
        if (anim == animation_name && !first_cycle)
        {
            // restore path from "root"
            graphics_logger->trace("searching parent for <{}:{}>", anim, frame_to);

            parent = visited_parent[make_pair(anim, frame_to)];
            while (parent.anim != "root")
            {
                graphics_logger->trace("parent <\"{}\":{}>", parent.anim, parent.frame);

                min_call_stack.push_front(parent.j);
                if (visited_parent.count(make_pair(parent.anim, parent.frame)) == 0)
                {
                    graphics_logger->error("Trying to get parent of non-existent anim: {}, frame: {}", parent.anim, parent.frame);
                    throw;
                }
                parent = visited_parent[make_pair(parent.anim, parent.frame)];
            }

            // exit bfs
            break;
        }

        for (Joint j_next : animations.at(anim)->get_joints())
        {
            // We have starting frame: j.frame_to
            if (j_next.frame < j.frame_to)
                continue;

            // time from beginning of animation to joint
            duration = j_next.frame - frame_to;
            // now we find new place to insert joint
            std::size_t i = 0;
            for (; i < visited_frame.size(); ++i)
                if (frame + duration < visited_frame[i])
                    break;

            // update visited, etc. only if j_next is to be visited by earlier frame_to
            if (visited_fromframe.count(j_next.anim_to) == 0 || visited_fromframe[j_next.anim_to] > j_next.frame_to)
            {
                visited_fromframe[j_next.anim_to] = j_next.frame_to;
                visited.insert(next(visited.begin(), i), j_next);
                visited_frame.insert(next(visited_frame.begin(), i), frame + duration);
                int ft = frame_to;
                if (first_cycle)
                    ft = -1;

                visited_parent[make_pair(j_next.anim_to, j_next.frame_to)] = ParentJoint{anim, ft, j_next};

                graphics_logger->trace("+ adding parent for <{}, {}:{}> with <{}:{}>", j_next.frame, j_next.anim_to, j_next.frame_to, anim, ft);
                graphics_logger->trace("so, parent for <{}:{}> is <{}:{}>", j_next.anim_to, j_next.frame_to,
                                       visited_parent[make_pair(j_next.anim_to, j_next.frame_to)].anim,
                                       visited_parent[make_pair(j_next.anim_to, j_next.frame_to)].frame);
            }
        }

        first_cycle = false;
    }

    graphics_logger->trace("found minimum frame with call stack of size {}", min_call_stack.size());
    return min_call_stack;
}

// add animation to map by name
void Character::add_animation(string animation_name, Animation* p_animation)
{
    animations[animation_name] = p_animation;
    for (Joint j : p_animation->get_joints())
    {
        graphics_logger->trace("Character: adding pair of anim: {}:{} -> {}:{}", animation_name, j.frame, j.anim_to, j.frame_to);

        // <animation_name> can be followed by <j.anim_to> and vice versa
        animation_subsequent[animation_name].insert(j.anim_to);
        animation_previous[j.anim_to].insert(animation_name);
    }
}

// set current animation by name
void Character::set_animation(string animation_name, Time offset, int frame_start, int frame_stop_after)
{
    // if no valid animation is passed, then go to idle
    if (animation_name == "")
    {
        set_animation_to_idle(offset);
        return;
    }

    graphics_logger->debug("Character::setting animation to {} at frame {}", animation_name, frame_start);

    current_animation = animation_name;

    base_sprite->play(animations[animation_name], frame_start, offset);
    base_sprite->stop_after(frame_stop_after);

    animated = true;
    base_sprite->setLooped(false);
}

// set current animation to idle with current direction
void Character::set_animation_to_idle(Time offset)
{
    string animation_name = get_idle_animation_s(facing_direction);
    graphics_logger->trace("Character::trying to set idle animation to {} since curdir is {}", animation_name, facing_direction);
    if (animations.count(animation_name) == 0)
        animation_name = idle_animation;

    graphics_logger->debug("Character::setting idle animation to {}", animation_name);

    current_animation = animation_name;

    base_sprite->play(animations[animation_name], offset);
    animated = true;
    base_sprite->setLooped(true);
}

// scheduled movement direction
// need for smooth animation transitions
int Character::get_next_movement_direction() const
{
    return next_animation_dir;
}
// if next movement is scheduled
bool Character::is_next_movement_planned() const
{
    return next_movement_planned;
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

    deque<Joint> next_joints;
    // if ignoring joints, then instantly process to target animation
    if (ignore_joints)
    {
        // since plan_movement is called before Character::update, we can stop at current frame
        next_joints.clear();
        int next_stop;
        // if VE active, then don't interrupt it
        if (base_sprite != moving_sprite)
            next_stop = (int)base_sprite->getAnimation()->getSize() - 1;
        else
            next_stop = (int)base_sprite->getFrame() + 1;
        next_stop = min(next_stop, (int)base_sprite->getAnimation()->getSize() - 1);

        next_joints.push_back(Joint{next_stop, animation_name, 1});

        graphics_logger->debug("Character::plan_movement ignoring joints with joint: [F:{}, A:{}, T:{}]",
                               next_joints.back().frame, next_joints.back().anim_to, next_joints.back().frame_to);
    }
    else
    {
        next_joints = find_next_joint(animation_name);

        graphics_logger->debug("Character::plan_movement found queue of {}", next_joints.size());

        // if no path to target animation_name found, then break
        if (next_joints.size() == 0)
            return;

        string s = "";
        for (Joint j : next_joints)
        {
            s += " {F: " + std::to_string(j.frame) + ", A: " + j.anim_to + "}";
        }
        graphics_logger->debug(s);
    }

    // otherwise, construct queue
    next_animations.clear();
    std::size_t i = 0;
    Joint j;
    for (; i < next_joints.size() - 1; ++i)
    {
        j = next_joints[i];
        graphics_logger->trace("Character::plan_movement: pushing joint to {}", j.anim_to);

        AnimMovement am;
        am.direction = direction;
        am.shift = Vector2f(0, 0);
        am.animation = j.anim_to;
        am.VE_allowed = false;
        am.has_VE = false;
        // if next anim is target (and moving) and it begins with skipping frames, then begin VE in advance
        if (shift != Vector2f(0, 0) && next_joints[i + 1].anim_to == animation_name && next_joints[i + 1].frame_to > 1)
        {
            am.VE_allowed = true;
            am.has_VE = true;
            am.VE_start = base_sprite->getPosition(); /// is valid ?
            am.VE_shift = shift;
            am.VE_duration = duration;

            graphics_logger->trace("with VE");
        }
        am.j = next_joints[i];
        am.jnext = next_joints[i + 1];

        next_animations.push_back(am);
    }

    // last AnimMovement
    {
        j = next_joints.back();
        graphics_logger->trace("Character::plan_movement: pushing final joint to {}", j.anim_to);

        AnimMovement am;
        am.direction = direction;
        am.shift = Vector2f(0, 0);
        am.animation = j.anim_to;
        am.VE_allowed = true;
        am.has_VE = false;
        // if this (target) anim is moving and previous (if present) anim does not construct VE, then begin VE
        if (shift != Vector2f(0, 0) && (next_animations.size() == 0 || !next_animations.back().has_VE))
        {
            am.has_VE = true;
            am.VE_start = base_sprite->getPosition(); /// is valid ?
            am.VE_shift = shift;
            am.VE_duration = duration;
        }
        am.j = next_joints.back();
        am.jnext = Joint({-1, "", 1});

        next_animations.push_back(am);

        next_animation_dir = direction;
        next_movement_planned = true;
    }

    // set stop_after in current animation to new joint
    base_sprite->stop_after(next_joints.front().frame);
}

// pops and plays an animation from AnimMovement deque (VEs as well)
void Character::next_movement_start()
{
    AnimMovement am = next_animations.front();

    graphics_logger->debug("Character::next_movement_start, has_VE={}, and direction {}", am.has_VE, am.direction);
    graphics_logger->trace("Paused? {}, time after stop {}", !base_sprite->isPlaying(), base_sprite->time_after_stop().asSeconds());

    next_animations.pop_front();
    set_animation(am.animation, base_sprite->time_after_stop(), am.j.frame_to, am.jnext.frame);

    // we change facing direction only when VE starts
    // if (am.direction != -1)
    //     set_facing_direction(am.direction);

    if (am.has_VE)
    {
        Time offset = seconds(0);
        if (am.jnext.frame != -1)
        {
            offset += seconds(am.jnext.frame_to * base_sprite->getFrameTime().asSeconds());
            offset -= seconds((am.jnext.frame - am.j.frame_to) * base_sprite->getFrameTime().asSeconds());
        }
        if (moving_sprite != base_sprite)
        {
            graphics_logger->error("Character::next_movement_start: Trying to stack VE");
            throw;
        }

        graphics_logger->debug("Creating VisualEffect with offset {}", offset.asSeconds());

        VisualEffect* VE = new VisualEffect(base_sprite, offset, am.VE_duration, am.VE_start, am.VE_start + am.VE_shift);
        // Sync legs animation with VisualEffect start
        VE->sprite->play(am.j.frame_to, offset);

        moving_sprite = VE;
        moving_sprite->play();

        // we change facing direction only when VE starts
        if (am.direction != -1)
            set_facing_direction(am.direction);

        // now we count this movement as finished: ready to receive new orders
        is_order_completed = true;
        next_animation_dir = -1;
        next_movement_planned = false;

        moving_shift = am.VE_shift;
        moving = true;
    }
}

// stop and delete VE forcefully
void Character::stop_movement_by_force()
{
    // unwrap VE
    if (moving_sprite != base_sprite)
    {
        graphics_logger->debug("Deleting VisualEffect by force");

        delete moving_sprite;
        moving_sprite = base_sprite;

        moving = false;
    }
}

// flush animation queue by force
void Character::stop_animation_by_force()
{
    // cancel all scheduled animations
    next_animations.clear();
    // stop current animation
    base_sprite->stop();

    // the rest is up to update
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
        graphics_logger->debug("Deleting VisualEffect");

        delete moving_sprite;
        moving_sprite = base_sprite;

        moving = false;
    }

    if (!base_sprite->isPlaying())
    {
        // start next animation if no VE or if supports VE
        if (next_animations.size() > 0)
        {
            // we start next animation in two cases: it has VE and we don't move and it has no VE and either (we move and VE is allowed), either no move
            bool has_VE_correct_start = next_animations.front().has_VE && !moving;
            bool no_VE_correct_start = !next_animations.front().has_VE && moving && next_animations.front().VE_allowed;
            if (!moving || has_VE_correct_start || no_VE_correct_start)
            {
                graphics_logger->trace("Character::update; Moving? {}; b==m? {}", moving, (base_sprite==moving_sprite));

                next_movement_start();
            }
        }
        else
        {
            graphics_logger->trace("Character::update setting animation to idle");
            next_movement_planned = false;
            // set to idle if no next animation scheduled
            if (!moving)
                set_animation_to_idle(base_sprite->time_after_stop());
        }
    }

    moving_sprite->update(deltaTime);
}

void Character::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(*moving_sprite);
}
