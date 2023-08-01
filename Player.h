#ifndef PLAYER_INCLUDE
#define PLAYER_INCLUDE

#include <iostream>
#include <typeinfo>

#include "AnimatedSprite.h"
#include "Character.h"
#include "extra_algorithms.h"

#include <spdlog/spdlog.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

class Field;

using namespace sf;

// Player binds keyboard input with animated Character
// Player contains pressed directions, blocking information and player coordinates
// When arrows or WASD pressed, call <add_movement_direction> when released, call <release_movement_direction> to cancel order
// You can manually move Player with move_player
// Transformable methods (setPosition, etc.) overriden to work with contained Character

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
    std::unique_ptr<Character> sprite;
    // removes "blocking checked" from movements queue
    void reset_blocking_check();

    std::shared_ptr<spdlog::logger> map_events_logger;

    // to update blocking correctly
    Field* current_field = NULL;

public:
    std::string name;
    // queued movements up-down-right-left
    deque<Movement> queued_movement_direction;

    // cell coords
    unsigned int x_cell_coord = 0, y_cell_coord = 0;
    // screen coords
    double x_cell_movement_coord = 0, y_cell_movement_coord = 0;
    // smooth movement flag
    bool movement_animation = true; /// NOT IMPLEMENTED (look Character::moving_enabled)

public:
    Player(std::string name);

    // current_field pointer setter/getter
    void set_current_field(Field* nfield);
    Field* get_current_field() const;

    // getter and setter for Character
    Character& getCharacter();
    void setCharacter(std::unique_ptr<Character> new_character);

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
    void setScale(const Vector2f &factors);
    FloatRect getLocalBounds() const;
    FloatRect getGlobalBounds() const;

    // overriding Drawable methods
    void update(Time deltaTime);
    void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // PLAYER_H
