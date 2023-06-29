#include "Cell_object.h"

Cell_object::Cell_object(std::string name, Texture* texture) : texture(texture), type_name(name)
{
    depth_level = 1;
}

Cell_object::~Cell_object()
{
   //std::cout << "went ";
}

// change m_vertices
void Cell_object::addTexCoords(IntRect rect)
{
    m_vertices[0].position = Vector2f(0.f, 0.f);
    m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
    m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
    m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

    float left = static_cast<float>(rect.left) + 0.0001f;
    float right = left + static_cast<float>(rect.width);
    float top = static_cast<float>(rect.top);
    float bottom = top + static_cast<float>(rect.height);

    m_vertices[0].texCoords = Vector2f(left, top);
    m_vertices[1].texCoords = Vector2f(left, bottom);
    m_vertices[2].texCoords = Vector2f(right, bottom);
    m_vertices[3].texCoords = Vector2f(right, top);
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
