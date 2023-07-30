#ifndef CELL_INCLUDE
#define CELL_INCLUDE

#include <map>
#include <iostream>
#include <typeinfo>
#include <bitset>

#include "AnimatedSprite.h"
#include "Cell_object.h"
#include "extra_algorithms.h"

#include <jsoncpp/json.h>
#include <spdlog/spdlog.h>
#include <SFML/Graphics/Sprite.hpp>

using namespace sf;

class Cell : public Drawable, public Transformable
{
private:
    // sprite to display texture
    Sprite sprite;

    // placeable objects by name
    std::map <std::string, std::shared_ptr<Cell_object> > objects;

    // movement blocking mask flag
    // bits 1-4 means cell can't be entered from direction 0-3
    // bits 5-8 means cell can't be leaved with direction 0-3
    std::bitset<8> blocking;

protected:
    std::shared_ptr<spdlog::logger> map_events_logger;

public:
    // tile type
    std::string type_name;
    // if sprite is displayed
    bool displayed;

    Cell(std::string name);
    Cell(std::string name, std::shared_ptr<Texture> background, IntRect texRect = IntRect(0, 0, 120, 120));
    // change tile texture and name
    void change_texture(std::string name, std::shared_ptr<Texture> texture);
    // set position to Cell and all child objects
    void set_position_recursive(double x, double y);
    // is object in object map by name
    bool hasObject(std::string name);
    // add object with name and z-coordinate
    std::shared_ptr<Cell_object> addObject(std::string name, std::shared_ptr<Texture> texture, int depth_level);
    std::shared_ptr<Cell_object> addObject(std::string name, std::shared_ptr<Texture> texture, IntRect tex_rect, int depth_level);
    std::shared_ptr<Cell_object> addObject(std::string name, std::shared_ptr<Texture> texture, Vector2f display_size, int depth_level);
    std::shared_ptr<Cell_object> addObject(std::string name, std::shared_ptr<Texture> texture, Vector2f display_size, IntRect tex_rect, int depth_level);
    // remove object from map by name
    void removeObject(std::string name);
    // invoke action by name. texture - temporary variable, used for creating <stump>
    void action_change(std::string name, std::shared_ptr<Texture> texture);
    // save cell and child objects to json
    void save_cell(unsigned int x, unsigned int y, Json::Value& Location);

    // set blocking
    void set_in_block(int direction, bool block=true);
    void set_out_block(int direction, bool block=true);
    // update blocking: change only if block = true
    void update_in_block(int direction, bool block=true);
    void update_out_block(int direction, bool block=true);
    // ask blocking
    bool has_in_block(int direction) const;
    bool has_out_block(int direction) const;

    // overriding Transformable methods
    void setPosition(const Vector2f &pos);
    void setPosition(float x, float y);

    // overriding Drawable methods
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    void draw_objects(RenderTarget& target, RenderStates states) const;

    /// TEMP
    int mapsize() const;
};

#endif // CELL_INCLUDE
