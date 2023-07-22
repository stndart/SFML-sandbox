#include "Cell_object.h"

Cell_object::Cell_object(std::string name, Texture* texture, IntRect rect) : texture(texture), type_name(name), depth_level(1)
{
    addTexCoords(rect);
}

// change m_vertices
void Cell_object::addTexCoords(IntRect rect)
{
    cutout_texture_to_frame(m_vertices, rect);
}

void Cell_object::draw(RenderTarget& target, RenderStates states) const
{
    if (texture)
    {
        states.transform *= getTransform();
        states.texture = texture;
        target.draw(m_vertices, 4, Quads, states);
    }
}
