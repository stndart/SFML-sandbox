#include "Player.h"

Player::Player(std::string name)
{
    // Reaching out to global "map_events" logger by names
    map_events_logger = spdlog::get("map_events");

    map_events_logger->debug("Constructed player, \"{}\" with no sprite", name);
}

// current_field pointer setter/getter
void Player::set_current_field(Field* nfield)
{
    current_field = nfield;

    // reset blocking and moving
    reset_blocking_check();
    for (int i = 0; i < 4; ++i)
        release_movement_direction(i);

    sprite->stop_movement_by_force();
    sprite->stop_animation_by_force();
}

Field* Player::get_current_field() const
{
    return current_field;
}

// removes "blocking checked" from movements queue
void Player::reset_blocking_check()
{
    for (auto r_iter = queued_movement_direction.rbegin();
        r_iter != queued_movement_direction.rend(); r_iter++)
    {
        r_iter->blocking_checked = false;
    }
}

// getter and setter for Character
Character& Player::getCharacter()
{
    return *sprite;
}

void Player::setCharacter(std::unique_ptr<Character> new_character)
{
    sprite = std::move(new_character);
}

// push back direction to queue
void Player::add_movement_direction(Vector2f shift, int direction)
{
    map_events_logger->trace("Adding player movement direction {}", direction);

    Movement m = {direction, shift, false, false};
    queued_movement_direction.push_back(m);
}

// pop all coinciding directions from queue
void Player::release_movement_direction(int direction)
{
    map_events_logger->trace("Releasing player movement direction {}", direction);

    std::erase_if(queued_movement_direction, [direction](Movement x) { return x.direction == direction; });
}

// check if direction is present in queue
bool Player::has_queued_direction(int direction)
{
    auto mov_dir_iter = queued_movement_direction.begin();
    for (; mov_dir_iter != queued_movement_direction.end(); mov_dir_iter++)
        if (mov_dir_iter->direction == direction)
            return true;
    return false;
}

// request sprite movement by shift. Direction is optional
void Player::move_player(Vector2f shift, int direction)
{
    map_events_logger->trace("Player requests movement in direction ", direction);

    sprite->plan_movement(shift, direction);
}

void Player::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    sprite->setPosition(position);
}

Vector2f Player::getPosition() const
{
    return sprite->getPosition();
}

void Player::setScale(const Vector2f &factors)
{
    Transformable::setScale(factors);
    sprite->setScale(factors);
}

FloatRect Player::getLocalBounds() const
{
    if (sprite)
        return sprite->getLocalBounds();
    return FloatRect(0, 0, 0, 0);
}

FloatRect Player::getGlobalBounds() const
{
    if (sprite)
        return sprite->getGlobalBounds();
    return FloatRect(0, 0, 0, 0);
}

void Player::update(Time deltaTime)
{
    int mov_dir = -1;
    Vector2f mov_shift = Vector2f(0, 0);
    // find next available direction
    if (queued_movement_direction.size() > 0)
    {
        // find next valid direction (not blocked movement)
        for (auto r_iter = queued_movement_direction.begin();
             r_iter != queued_movement_direction.end(); r_iter++)
        {
            if (r_iter->blocking_checked && !r_iter->blocked)
            {
                mov_dir = r_iter->direction;
                mov_shift = r_iter->shift;
            }
        }
    }

    // if planned movement is different (or blank)
    if (sprite->get_next_movement_direction() != mov_dir || !sprite->is_next_movement_planned())
    {
        // cancels previous planned move if present
        // if plan is impossible, silently passes. Eventually animations will return to idle and plan will pass then
        map_events_logger->trace("Player::update plan movement with dir: {}", mov_dir);
        map_events_logger->trace("because next_planned: {}, and next_dir: ", sprite->is_next_movement_planned(), sprite->get_next_movement_direction());

        sprite->plan_movement(mov_shift, mov_dir);
    }

    // if VE is started (function resets flag once called), update coords
    if (sprite->order_completed())
    {
        mov_shift = sprite->get_current_shift();
        mov_dir = direction_from_shift(mov_shift);
        // if it is not "standing"
        if (mov_shift != Vector2f(0, 0))
        {
            map_events_logger->debug("Player::update coords with movement from {}x{} with direction {}", x_cell_coord, y_cell_coord, mov_dir);

            x_cell_coord += direction_x[mov_dir];
            y_cell_coord += direction_y[mov_dir];
            // every time we move, check blocking again
            reset_blocking_check();
        }
    }

    sprite->update(deltaTime);
}

void Player::draw(RenderTarget& target, RenderStates states) const
{
    sprite->draw(target, states);
}