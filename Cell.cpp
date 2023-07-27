#include "Cell.h"

Cell::Cell(std::string name) : blocking(0), type_name(name)
{
    // Reaching out to global "map_events" logger by name
    map_events_logger = spdlog::get("map_events");
}

Cell::Cell(std::string name, Texture* texture, IntRect texRect) : blocking(0), type_name(name)
{
    map_events_logger = spdlog::get("map_events");

    sprite.setTexture(*texture);
    sprite.setTextureRect(texRect);
}

// change tile texture and name
void Cell::change_texture(std::string name, Texture* texture)
{
    type_name = name;
    sprite.setTexture(*texture);
}

// set position to Cell and all child objects
void Cell::set_position_recursive(double x, double y)
{
    setPosition(x, y);
    for (auto obj : objects)
    {
        obj.second->setPosition(0, 0);
    }
}

// is object in object map by name
bool Cell::hasObject(std::string name)
{
    auto obj = objects.find(name);
    return obj != objects.end();
}

// add object with name and z-coordinate
Cell_object* Cell::addObject(std::string name, Texture* texture, int depth_level)
{
    map_events_logger->debug("Adding object \"{}\" to cell with z-level {}", name, depth_level);

    Cell_object* new_object = new Cell_object(name, texture);

    /// Что за магическое число 120?
    if (name == "house")
        new_object->setDisplaySize(Vector2f(360, 240));
    else if (name == "table")
        new_object->setDisplaySize(Vector2f(240, 120));
    else
        new_object->setDisplaySize(Vector2f(120, 120));

    new_object->depth_level = depth_level;
    /// TODO: обработать, если objects[name] уже существует
    objects[name] = new_object;
    return new_object;
}

// remove object from map by name
void Cell::removeObject(std::string name)
{
    map_events_logger->debug("Removed object \"{}\" from cell", name);

    objects.erase(name);
}

// invoke action by name. texture - temporary variable, used for creating <stump>
void Cell::action_change(std::string name, Texture* texture)
{
    map_events_logger->debug("Action change on cell with action name {}", name);

    /// TODO: запихнуть все именные действия в json
    /// Что-то вроде "choptree" = remove: "tree", add: "stump"
    /// Этот метод превратится в "заменить %a на %b"
    removeObject(name);
    if (name == "tree")
    {
        addObject("stump", texture, 1);
    }
}

// save cell and child objects to json
void Cell::save_cell(unsigned int cell_x, unsigned int cell_y, nlohmann::json& Location)
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

// set blocking
void Cell::set_in_block(int direction, bool block)
{
    blocking[direction] = block;
}

void Cell::set_out_block(int direction, bool block)
{
    blocking[direction + 4] = block;
}

// ask blocking
bool Cell::has_in_block(int direction) const
{
    return blocking[direction];
}

bool Cell::has_out_block(int direction) const
{
    return blocking[direction + 4];
}

// overriding Transformable methods
void Cell::setPosition(const Vector2f &pos)
{
    Transformable::setPosition(pos);
    sprite.setPosition(pos);
}

void Cell::setPosition(float x, float y)
{
    Transformable::setPosition(x, y);
    sprite.setPosition(x, y);
}

void Cell::draw(RenderTarget& target, RenderStates states) const
{
    if (sprite.getTexture())
    {
        target.draw(sprite, states);
    }
}

void Cell::draw_objects(RenderTarget& target, RenderStates states) const
{
    states.transform *= getTransform();
    for (auto obj : objects)
    {
        obj.second->draw(target, states);
    }
}

int Cell::mapsize() const
{
    return objects.size();
}

