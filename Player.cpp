#include "Player.h"

Player::Player(std::string name) : name(name),
x_cell_coord(0), y_cell_coord(0),
queued_movement_direction(deque<Movement>()),
x_cell_movement_coord(0), y_cell_movement_coord(0), movement_animation(false)
{
    cout << "CONSTRUCTING player, name " << name << " with no sprite" << endl;

    sprite = NULL;
}

Player::Player(std::string name, Texture* texture, IntRect frame0) : name(name),
x_cell_coord(0), y_cell_coord(0),
queued_movement_direction(deque<Movement>()),
x_cell_movement_coord(0), y_cell_movement_coord(0), movement_animation(false)
{
    cout << "CONSTRUCTING player, name " << name << " sprite " << sprite << endl;

    sprite = new Character(name, *texture, frame0);
    sprite->set_moving_enabled(true);
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

bool Player::is_moving() const
{
    return sprite->is_moving();
}

// push back direction to queue
void Player::add_movement_direction(Vector2f shift, int direction)
{
    Movement m = {direction, shift, false, false};
    queued_movement_direction.push_back(m);
}

// pop all coinciding directions from queue
void Player::release_movement_direction(int direction)
{
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
    //std::cout << "PLayer requests movement\n";
    sprite->movement(shift, direction);
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

void Player::update(Time deltaTime)
{
    // if next movement is ready to be scheduled/invoked
    if (sprite->passedNoReturn())
    {
        int mov_dir = -1;
        Vector2f mov_shift = Vector2f(0, 0);
        // find next available direction. If no, proceed with standing still
        if (queued_movement_direction.size() > 0)
        {
            // std::cout << "trying to schedule smth\n";
            // find next valid direction (not blocked movement)
            for (auto r_iter = queued_movement_direction.rbegin();
                 r_iter != queued_movement_direction.rend(); r_iter++)
            {
                //std::cout << "block check: " << r_iter->blocking_checked << " block: " << r_iter->blocked << std::endl;
                if (r_iter->blocking_checked && !r_iter->blocked)
                {
                    mov_dir = r_iter->direction;
                    mov_shift = r_iter->shift;
                }
            }
        }

        // schedule/invoke movement
        // if no direction is scheduled, then process to idle animation only if sprite is moving
        if (sprite->is_moving() || mov_dir != -1)
        {
            // if we're requesting move, not stop
            if (mov_dir != -1)
            {
                std::cout << "=== Player::update movement from " << x_cell_coord << " " << y_cell_coord << " with dir: " << mov_dir << std::endl;
                // since point of no return is passed, change coords
                x_cell_coord += direction_x[mov_dir];
                y_cell_coord += direction_y[mov_dir];
                reset_blocking_check();
            }

            sprite->movement(mov_shift, mov_dir);
        }
    }

    sprite->update(deltaTime);
}

void Player::draw(RenderTarget& target, RenderStates states) const
{
    sprite->draw(target, states);
}

void Player::add_animation(string animation_name, Animation* p_animation)
{
    sprite->add_animation(animation_name, p_animation);
}
void Player::set_animation(string animation_name)
{
    sprite->set_animation(animation_name);
}
void Player::set_next_animation(string animation_name)
{
    sprite->set_next_animation(animation_name);
}
void Player::set_idle_animation(string animation_name)
{
    sprite->idle_animation = animation_name;
    sprite->moving_sprite->setLooped(true);
    sprite->set_animation(animation_name);
}
