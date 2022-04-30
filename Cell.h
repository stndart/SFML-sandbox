#ifndef CELL_INCLUDE
#define CELL_INCLUDE

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "AnimatedSprite.h"

using namespace sf;

class Cell : public Drawable, public Transformable
{
private:
    Vertex m_vertices[4];
    const Texture* background;
public:
    std::string name;
    Cell(std::string name);
    Cell(std::string name, Texture* background);
    void addTexture(Texture* texture);
    void addCoords(IntRect rect);
    void draw(RenderTarget& target, RenderStates states) const override;
};

Cell new_cell(Texture* bg, std::string name);

#endif // CELL_H
