#ifndef PLAYER_INCLUDE
#define PLAYER_INCLUDE

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "AnimatedSprite.h"

using namespace sf;

class Player : public Drawable, public Transformable
{
private:
    Vertex m_vertices[4];
    Texture* background;
public:
    std::string name;
    unsigned int x_cell_coord, y_cell_coord;
    double x_cell_movement_coord, y_cell_movement_coord;
    bool movement_animation;
    Player(std::string name, Texture* background);
    void addTexCoords(IntRect rect);
    void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // PLAYER_H
