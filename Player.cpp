#include "Player.h"
#include <iostream>
#include <typeinfo>

Player::Player(std::string name, Texture* texture, IntRect frame0) : name(name),
x_cell_coord(0), y_cell_coord(0),
x_cell_movement_coord(0), y_cell_movement_coord(0), movement_animation(false)
{
    sprite = new Character(name, *texture, frame0);
    sprite->set_moving_enabled(true);
}

void Player::move_player(Vector2f shift, int direction)
{
    sprite->movement(shift, direction);
}

bool Player::is_moving() const
{
    return sprite->is_moving();
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
