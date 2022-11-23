#include "Cell.h"
#include <iostream>
#include <typeinfo>

Cell::Cell(std::string name) : type_name(name)
{

}

Cell::Cell(std::string name, Texture* texture) : background(texture), type_name(name)
{
    addTexCoords(IntRect(0, 0, 120, 120));
}

void Cell::change_texture(std::string name, Texture* texture)
{
    background = texture;
    type_name = name;
}

void Cell::addTexCoords(IntRect rect)
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

void Cell::set_position_recursive(double x, double y)
{
    setPosition(x, y);
    for (auto obj : objects)
    {
        obj.second->setPosition(0, 0);
    }
}

bool Cell::hasObject(std::string name)
{
    auto obj = objects.find(name);
    return obj != objects.end();
}

Cell_object* Cell::addObject(std::string name, Texture* texture, int depth_level)
{
    Cell_object* new_object = new Cell_object(name, texture);
    /// Что за магическое число 120?
    new_object->addTexCoords(IntRect(0, 0, 120, 120));
    new_object->depth_level = depth_level;
    /// TODO: обработать, если objects[name] уже существует
    objects[name] = new_object;
    return new_object;
}

void Cell::removeObject(std::string name)
{
    objects.erase(name);
}

void Cell::action_change(std::string name, Texture* texture)
{
    /// TODO: запихнуть все именные действия в json
    /// Что-то вроде "choptree" = remove: "tree", add: "stump"
    /// Этот метод превратится в "заменить %a на %b"
    removeObject(name);
    if (name == "tree")
    {
        addObject("stump", texture, 1);
    }
}

void Cell::save_cell(unsigned int cell_x, unsigned int cell_y, Json::Value& Location)
{
    Location["map"][cell_x][cell_y]["type"] = type_name;
    if (objects.size() == 0)
    {
        Location["big_objects"][cell_x][cell_y];
    }

    unsigned int i = 0;
    for (auto obj : objects)
    {
        std::string tname = obj.second->type_name;
        int zindex = obj.second->depth_level;
        Location["big_objects"][cell_x][cell_y][i]["type"] = tname;
        Location["big_objects"][cell_x][cell_y][i]["depth_level"] = zindex;
        i++;
    }
}

void Cell::draw(RenderTarget& target, RenderStates states) const
{
    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }

    for (auto obj : objects)
    {
        obj.second->draw(target, states);
    }
}
