#include "Player.h"

Player::Player(std::string name, Texture* texture, IntRect frame0) : name(name),
x_cell_coord(0), y_cell_coord(0),
x_cell_movement_coord(0), y_cell_movement_coord(0), movement_animation(false)
{
    sprite = new Character(name, *texture, frame0);
    sprite->set_moving_enabled(true);
    queued_movement_direction = deque<Movement>();

    cout << "CONSTRUCTING player, name " << name << " sprite " << sprite << endl;
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
    auto mov_dir_iter = queued_movement_direction.begin();
    for (; mov_dir_iter != queued_movement_direction.end(); mov_dir_iter++)
    {
        if (mov_dir_iter->direction == direction)
        {
            break;
        }
    }
    if (mov_dir_iter == queued_movement_direction.begin())
    {
        sprite->cancel_next_movement();
    }
    queued_movement_direction.erase(mov_dir_iter);
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

void Player::update(Time deltaTime)
{
    //cout << "Player checking next move: scheduled move present? " << sprite->has_next_movement() << " mov queue size " << queued_movement_direction.size() << endl;

    //cout << name << " " << sprite << endl;
    if (!sprite->has_next_movement() && sprite->switched_to_next_animation)
    {
        //cout << "has anim and switched " << sprite->has_next_movement() << " " << sprite->switched_to_next_animation << endl;
        //cout << "Arrived at " << x_cell_coord << " " << y_cell_coord << endl;

        sprite->switched_to_next_animation = false;
        int mov_dir = sprite->get_current_direction();

        //cout << "PLayer moving from " << x_cell_coord << " " << y_cell_coord << " as started movement\n";
        x_cell_coord += direction_x[mov_dir];
        y_cell_coord += direction_y[mov_dir];
        reset_blocking_check();
    }

    if (!sprite->has_next_movement() && queued_movement_direction.size() > 0)
    {
        int mov_dir = -1;
        Vector2f mov_shift = Vector2f(0, 0);
        for (auto r_iter = queued_movement_direction.rbegin();
             r_iter != queued_movement_direction.rend(); r_iter++)
        {
            //cout << "Player checking direction " << r_iter->direction << " is checked " << r_iter->blocking_checked << " and blocked " << r_iter->blocked << endl;
            if (r_iter->blocking_checked && !r_iter->blocked)
            {
                mov_dir = r_iter->direction;
                mov_shift = r_iter->shift;
            }
        }
        if (mov_dir != -1)
        {
            //cout << "-- MOVING direction " << mov_dir << " shift " << mov_shift.x << " " << mov_shift.y << " from " << x_cell_coord << " " << y_cell_coord << endl;
            if (!sprite->is_moving() && !sprite->has_next_movement())
            {
                //cout << "moving from " << x_cell_coord << " " << y_cell_coord << " as brand new movement\n";
                x_cell_coord += direction_x[mov_dir];
                y_cell_coord += direction_y[mov_dir];
            }

            sprite->movement(mov_shift, mov_dir);
            reset_blocking_check();
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
