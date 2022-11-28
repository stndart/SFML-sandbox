#ifndef PLAYER_INCLUDE
#define PLAYER_INCLUDE

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "AnimatedSprite.h"
#include "Character.h"

using namespace sf;

class Player : public Drawable, public Transformable
{
private:
    Character* sprite;
    deque<pair<int, Vector2f> > movement_direction;

public:
    std::string name;
    unsigned int x_cell_coord, y_cell_coord;
    double x_cell_movement_coord, y_cell_movement_coord;
    bool movement_animation;
    Player(std::string name, Texture* texture, IntRect frame0);
    bool is_moving() const;

    void add_movement_direction(Vector2f shift, int direction);
    void release_movement_direction(int direction);

    void move_player(Vector2f shift, int direction=-1);
    void setPosition(const Vector2f &position);
    Vector2f getPosition() const;
    void update(Time deltaTime);
    void draw(RenderTarget& target, RenderStates states) const override;

    /// TEMP
    void add_animation(string animation_name, Animation* p_animation);
    void set_animation(string animation_name);
    void set_next_animation(string animation_name);
    void set_idle_animation(string animation_name);
};

#endif // PLAYER_H
