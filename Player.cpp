#include "Player.h"
#include <iostream>
#include <typeinfo>

Player::Player(std::string name, Texture* texture, IntRect frame0) : name(name),
x_cell_coord(0), y_cell_coord(0),
x_cell_movement_coord(0), y_cell_movement_coord(0), movement_animation(false)
{
    sprite = new Character(name, *texture, frame0);
    sprite->set_moving_enabled(true);
    movement_direction = deque<pair<int, Vector2f> >();
}

void Player::move_player(Vector2f shift, int direction)
{
    sprite->movement(shift, direction);
}

bool Player::is_moving() const
{
    return sprite->is_moving();
}

void Player::add_movement_direction(Vector2f shift, int direction)
{
    //cout << "Player: adding movement direction\n";
    movement_direction.push_back(pair<int, Vector2f>({direction, shift}));

    //cout << "Add mov dir size " << movement_direction.size() << endl;
}

void Player::release_movement_direction(int direction)
{
    auto mov_dir_iter = movement_direction.begin();
    for (; mov_dir_iter != movement_direction.end(); mov_dir_iter++)
    {
        if (mov_dir_iter->first == direction)
        {
            break;
        }
    }
    if (mov_dir_iter + 1 == movement_direction.end())
    {
        //cout << "realeasing last\n";
        sprite->cancel_next_movement();
    }
    movement_direction.erase(mov_dir_iter);

    //cout << "Release mov dir size " << movement_direction.size() << endl;
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
    //cout << "checking next move: next move present? " << sprite->has_next_movement() << " mov dir size " << movement_direction.size() << endl;
    if (!sprite->has_next_movement() && movement_direction.size() > 0)
    {
        int mov_dir = movement_direction.back().first;
        Vector2f mov_shift = movement_direction.back().second;
        //cout << "MOVING direction " << mov_dir << " shift " << mov_shift.x << " " << mov_shift.y << endl;
        sprite->movement(mov_shift, mov_dir);
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
