#include "Cell.h"
#include <iostream>
#include <typeinfo>

Cell::Cell(std::string name) : name(name)
{

}

Cell::Cell(std::string name, Texture* texture) : background(texture), name(name)
{

}

Cell::Cell(std::string name, Texture* texture, Texture* texture1) : background(texture), name(name)
{
    /*
    Cell_object* object1 = new Cell_object("tree", texture1);
    objects.push_back(object1);
    */
}

void Cell::addTexture(Texture* texture)
{
    background = texture;
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

void Cell::addPosition(float x, float y)
{
    setPosition(x, y);
    for (auto it = objects.begin(); it != objects.end(); it++)
    {
        it->second->setPosition(0, 0);
        //std::cout << x << " " << y << std::endl;
    }
}

bool Cell::hasObject(std::string name)
{
    auto it = objects.find(name);
    if (it != objects.end())
    {
        //std::cout << "[" << name << "] = [" << objects.size() << "]" << std::endl;
        return true;
    }
    return false;
}

void Cell::addObject(Texture* texture, std::string name, int depth_level)
{
    Cell_object* object1 = new Cell_object(name, texture);
    object1->addTexCoords(IntRect(0, 0, 120, 120));
    object1->depth_level = depth_level;
    objects[name] = object1;
    //std::cout << objects.size() << " ";
}

void Cell::removeObject(std::string name)
{
    objects.erase(name);
}

void Cell::action_change(std::string name, Texture* texture)
{
    removeObject(name);
    addObject(texture, "S", 1);
}

void Cell::save_cell(unsigned int x, unsigned int y, Json::Value& Location)
{
    Location["map"][x][y]["type"] = name;
    if (objects.size() == 0)
    {
        Location["big_objects"][x][y];
    }
    auto current_object = objects.begin();
    int i = 0;
    while (current_object != objects.end())
    {
        Location["big_objects"][x][y][i]["type"] = current_object->second->type_name;
        Location["big_objects"][x][y][i]["depth_level"] = current_object->second->depth_level;
        current_object++;
        i++;
    }
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
        //std::cout << getPosition().x << " " << getPosition().y << std::endl;
    }

    if (name == "8")
    {
        //std::cout <<  objects.size() << " ";
    }
    for (auto it = objects.begin(); it != objects.end(); it++)
    {
        it->second->draw(target, states);
        //std::cout << it->second->type_name << std::endl;
    }
}

Cell* new_cell(Texture* bg, std::string name)
{
    Cell* cell = new Cell(name);
    cell->addTexture(bg);
    cell->addTexCoords(IntRect(0, 0, 120, 120));

    return cell;
}
