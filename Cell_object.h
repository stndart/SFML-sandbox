#ifndef CELL_OBJECT_INCLUDE
#define CELL_OBJECT_INCLUDE

#include <SFML/Graphics/Sprite.hpp>
#include "AnimatedSprite.h"

class Cell_object : public Drawable, public Transformable
{
private:
    // texture corners on texture image
    Vertex m_vertices[4];
    const Texture* texture;
public:
    // object type, eg. <stump>
    std::string type_name;
    // z-coordinate
    int depth_level;

    Cell_object(std::string name, Texture* texture);
    ~Cell_object();

    // change m_vertices
    void addTexCoords(IntRect rect);
    // overriding Drawable methods
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};
#endif // CELL_OBJECT_INCLUDE
