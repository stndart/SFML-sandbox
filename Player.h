#ifndef PLAYER_INCLUDE
#define PLAYER_INCLUDE

#include <iostream>
#include <typeinfo>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "extra_algorithms.h"

#include "AnimatedSprite.h"
#include "Character.h"

using namespace sf;

struct Movement
{
    int direction;
    Vector2f shift;
    bool blocked;
    bool blocking_checked;
};

class Player : public Drawable, public Transformable
{
private:
    Character* sprite;
    // removes "blocking checked" from movements queue
    void reset_blocking_check();

public:
    std::string name;
    // cell coords
    unsigned int x_cell_coord, y_cell_coord;
    // queued movements up-down-right-left
    deque<Movement> queued_movement_direction;

    // screen coords
    double x_cell_movement_coord, y_cell_movement_coord;
    // smooth movement flag
    bool movement_animation;

    Player(std::string name, Texture* texture, IntRect frame0);
    bool is_moving() const;

    // push back direction to queue
    void add_movement_direction(Vector2f shift, int direction);
    // pop all coinciding directions from queue
    void release_movement_direction(int direction);
    // check if direction is present in queue
    bool has_queued_direction(int direction);

    // request sprite movement by shift. Direction is optional
    void move_player(Vector2f shift, int direction=-1);

    // overriding Transformable methods
    void setPosition(const Vector2f &position);
    Vector2f getPosition() const;

    // overriding Drawable methods
    void update(Time deltaTime);
    void draw(RenderTarget& target, RenderStates states) const override;

    /// TEMP
    // interface to sprite methods
    void add_animation(string animation_name, Animation* p_animation);
    void set_animation(string animation_name);
    void set_next_animation(string animation_name);
    void set_idle_animation(string animation_name);
};

#endif // PLAYER_H
