#ifndef CELL_OBJECT_INCLUDE
#define CELL_OBJECT_INCLUDE

#include <SFML/Graphics/Sprite.hpp>
#include "AnimatedSprite.h"

class Cell_object : public Drawable, public Transformable
{
private:
    Vertex m_vertices[4];
    const Texture* texture;
public:
    std::string type_name;
    int depth_level;
    Cell_object(std::string name, Texture* texture);
    ~Cell_object();
    void addTexCoords(IntRect rect);
    void draw(RenderTarget& target, RenderStates states) const override;
};
#endif // CELL_OBJECT_INCLUDE
