#ifndef CHARACTER_INCLUDE
#define CHARACTER_INCLUDE

#include <iostream>
#include <string>
#include <deque>
#include <map>
#include <set>
#include <memory>

#include "Animation.h"
#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "extra_algorithms.h"

#include <spdlog/spdlog.h>
#include <SFML/System/Time.hpp>

using namespace std;
using namespace sf;

// Character is a structure, which contains player movement logic, as well as complicated animations sequencing mechanics
// Character has AnimatedSprite as a backbone and stores different Animations, which are activated with certain actions.
// As Animations support Joints (possible animation binding frames), Character searches for a shortest sequence of Animations to switch to desired Animation.
// Found Animation sequences are storedin <next_animations> and activated one after another automatically
// Method <plan_movement> provides logic for animation sequencing when Character needs to be moved by <shift>
// Joints mechanic is optional and can be disabled. In this case, Character provides moving and displaying underlying AnimatedSprite.
// Featured methods:
// set_facing_direction: sets direction to which Character needs to be faced. It changes default idle animation, but won't have an impact immediately
// add_animation: registers animation by name, otherwise it won't be scheduled when needed
// plan_movement: saves next move (by shift in pixels) or animation as scheduled animation. If Joints enabled, transition is set and played automatically
// order_completed: states if scheduled movement reached it's screen movement part. Resets flag after call

struct AnimMovement
{
    int direction;
    Vector2f shift;
    string animation; // optional
    bool VE_allowed;
    bool has_VE;
    Vector2f VE_start; // optional
    Vector2f VE_shift; // optional
    Time VE_duration; // optional
    Joint j;
    Joint jnext; // optional
};

// get default name of idle animation with direction
string get_idle_animation_s(int direction);
// get default name of movement animation with direction
string get_movement_animation_s(int direction);

class Character : public Drawable, public Transformable
{
private:
    // flag if VE is active
    bool moving;
    // flag if smooth movement is enabled (teleports otherwise)
    /// DEPRECATED: backward compatibility required
    bool moving_enabled;
    // flag if has valid animation
    bool animated;
    // flag if VE is started. Resets after call eponymous func
    bool is_order_completed;
    // flag ignoring joints (default: off)
    bool ignore_joints;

    // current animation name
    string current_animation;
    // animation name ends with facing direction: "movement_0"
    int facing_direction; // standard 0 - right, 1 - down, 2 - left, 3 - up
    // direction and shift of current VE
    int moving_direction;
    Vector2f moving_shift;
    // flag if Player needs to update self coordinates. Resets once checked
    bool movement_started;
    // flag if next movement is scheduled
    bool next_movement_planned;

    // scheduled movement parameters
    deque<AnimMovement> next_animations;
    // next animation direction (main one, transitions skipped). -1 if not scheduled.
    int next_animation_dir;

    // stores animations by name
    map<string, std::shared_ptr<Animation> > animations;
    // maps animation by name with follow up animations
    map<string, set<string> > animation_subsequent;
    // maps animation by name with preceding animations
    map<string, set<string> > animation_previous;

    string get_animation_name_by_shift(Vector2f shift, int dir=-1, string animation_name="") const;
    // find last joint of transition between <current_animation> and <animation_name>
    Joint last_joint(string animation_name="") const;
    // find next joint of transition between <current_animation> and <animation_name>
    // returns whole path to <animation_name> with next joing being the first in the deque
    // implements BFS
    deque<Joint> find_next_joint(string animation_name) const;

    std::shared_ptr<spdlog::logger> map_events_logger, graphics_logger;

public:
    string name;

    AnimatedSprite* base_sprite;

    AnimatedSprite* moving_sprite;
    // default animation by name
    // DEPRECATED ?
    string idle_animation;

    // set facing direction and change idle animation by direction (default: idle_animation_0)
    // DEPRECATED(set) ?
    void set_facing_direction(int new_direction);
    int get_facing_direction() const;

    // flag if movement is active
    bool is_moving() const;
    // flag if smooth movement is enabled (teleports otherwise)
    bool is_moving_enabled() const;
    void set_moving_enabled(bool enabled);
    // flag if has valid animation
    bool is_animated() const;

    // ignoring joints flag
    bool is_ignore_joints() const;
    void set_ignore_joints(bool ignore);

public:
    Character(string name, Texture* texture_default, IntRect texrect, FloatRect posrect);
    Character(string name, map<string, std::shared_ptr<Animation> > animations, FloatRect posrect); /// NOT IMPLEMENTED

    // add animation to map by name
    void add_animation(string animation_name, std::shared_ptr<Animation> p_animation);
    // set current animation by name with time offset
    void set_animation(string animation_name, Time offset = seconds(0), int frame_start = 0, int frame_stop_after = -1);
    // set current animation to idle with current direction. (<offset> uses last animation lag, for smoother transition)
    void set_animation_to_idle(Time offset = seconds(0));

    // scheduled movement direction
    // need for smooth animation transitions
    int get_next_movement_direction() const;
    // if next movement is scheduled
    bool is_next_movement_planned() const;
    // if VE just started. Resets flag after call
    bool order_completed();

    // gets current movement info
    int get_current_direction() const;
    string get_current_animation() const;
    Vector2f get_current_shift() const;

    // schedules next movement. If planned movement is already late to cancel, plans after it
    void plan_movement(Vector2f shift, int direction = -1, string animation_name = "", Time duration = seconds(0.4));
    // pops and plays an animation from AnimMovement deque (VEs as well)
    void next_movement_start();

    // stop and delete VE forcefully
    void stop_movement_by_force();
    // flush animation queue by force
    void stop_animation_by_force();


    // overriding Transformable methods
    void move(const Vector2f &offset);
    void rotate(float angle);
    void scale(const Vector2f &factor);
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;
    virtual const Vector2f& getPosition() const;
    virtual float getRotation() const;
    virtual const Vector2f& getScale() const;
    virtual void setPosition(const Vector2f &position);
    virtual void setPosition(float x, float y);
    virtual void setScale(const Vector2f &factors);
    virtual void setScale(float x, float y);
    virtual void setOrigin(const Vector2f &origin);
    virtual void setOrigin(float x, float y);

    // overriding Drawable methods
    virtual void update(Time deltaTime = seconds(0));
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif
