#include "Cell.h"
#include <iostream>
#include <typeinfo>

Cell::Cell(std::string name) : name(name)
{

}

Cell::Cell(std::string name, Texture* texture) : background(texture), name(name)
{

}

void Cell::addTexture(Texture* texture)
{
    background = texture;
}

void Cell::addCoords(IntRect rect)
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

void Cell::draw(RenderTarget& target, RenderStates states) const
{
    //std::cout << "Who asked " << name << " to draw?\n";
    //std::cout << name << " draw pos " << getPosition().x << " " << getPosition().y << std::endl;

    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }
}

Cell new_cell(Texture* bg, std::string name)
{
    Cell cell(name);
    cell.addTexture(bg);
    cell.addCoords(IntRect(0, 0, 120, 120));

    return cell;
}
