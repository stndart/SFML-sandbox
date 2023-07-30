#include "Cell.h"

Cell::Cell(std::string name) : blocking(0), type_name(name), displayed(true)
{
    // Reaching out to global "map_events" logger by name
    map_events_logger = spdlog::get("map_events");
}

Cell::Cell(std::string name, std::shared_ptr<Texture> texture, IntRect texRect) : Cell::Cell(name)
{
    map_events_logger = spdlog::get("map_events");

    for (int i = 0; i < 8; ++i)
    {
        blocking[i] = false;
    }

    sprite.setTexture(*texture);
    sprite.setTextureRect(texRect);
}

// change tile texture and name
void Cell::change_texture(std::string name, std::shared_ptr<Texture> texture)
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
std::shared_ptr<Cell_object> Cell::addObject(std::string name, std::shared_ptr<Texture> texture, Vector2f display_size, IntRect tex_rect, int depth_level)
{
    map_events_logger->trace("Adding object \"{}\" to cell with z-level {}", name, depth_level);

    std::shared_ptr<Cell_object> new_object = std::make_shared<Cell_object>(name, texture);

    new_object->addTexCoords(tex_rect);
    new_object->setDisplaySize(display_size);

    new_object->depth_level = depth_level;
    /// TODO: обработать, если objects[name] уже существует
    objects[name] = new_object;
    return new_object;
}

std::shared_ptr<Cell_object> Cell::addObject(std::string name, std::shared_ptr<Texture> texture, Vector2f display_size, int depth_level)
{
    IntRect tex_rect(Vector2i(0, 0), Vector2i(texture->getSize()));
    return addObject(name, texture, display_size, tex_rect, depth_level);
}

std::shared_ptr<Cell_object> Cell::addObject(std::string name, std::shared_ptr<Texture> texture, IntRect tex_rect, int depth_level)
{
    Vector2f display_size(texture->getSize());
    return addObject(name, texture, display_size, tex_rect, depth_level);
}

std::shared_ptr<Cell_object> Cell::addObject(std::string name, std::shared_ptr<Texture> texture, int depth_level)
{
    IntRect tex_rect(Vector2i(0, 0), Vector2i(texture->getSize()));
    Vector2f display_size(texture->getSize());
    return addObject(name, texture, display_size, tex_rect, depth_level);
}


// remove object from map by name
void Cell::removeObject(std::string name)
{
    map_events_logger->debug("Removed object \"{}\" from cell", name);

    objects.erase(name);
}

// invoke action by name. texture - temporary variable, used for creating <stump>
void Cell::action_change(std::string name, std::shared_ptr<Texture> texture)
{
    map_events_logger->debug("Action change on cell with action name {}", name);

    /// TODO: запихнуть все именные действия в json
    /// Что-то вроде "choptree" = remove: "tree", add: "stump"
    /// Этот метод превратится в "заменить %a на %b"
    removeObject(name);
    if (name == "tree")
    {
        addObject("stump", texture, IntRect(0, 0, 120, 120), 1);
    }
}

// save cell and child objects to json
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

// set blocking
void Cell::set_in_block(int direction, bool block)
{
    if (type_name == "table")
        map_events_logger->trace("Setting {} IN blocking {} in direction {}", type_name, block, direction);

    blocking[direction] = block;
}

void Cell::set_out_block(int direction, bool block)
{
    if (type_name == "table")
        map_events_logger->trace("Setting {} OUT blocking {} in direction {}", type_name, block, direction);

    blocking[direction + 4] = block;
}

// update blocking: change only if block = true
void Cell::update_in_block(int direction, bool block)
{
    if (block)
        set_in_block(direction, block);
}

void Cell::update_out_block(int direction, bool block)
{
    if (block)
        set_out_block(direction, block);
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
    if (sprite.getTexture() && displayed)
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

