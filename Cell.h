#ifndef CELL_INCLUDE
#define CELL_INCLUDE

#include <map>

#include <SFML/Graphics/Sprite.hpp>
#include "AnimatedSprite.h"
#include "Cell_object.h"
#include "json/json.h"

using namespace sf;

class Cell : public Drawable, public Transformable
{
private:
    // texture corners on background texture
    Vertex m_vertices[4];
    const Texture* background;
    // placeable objects by name
    std::map <std::string, Cell_object*> objects;
public:
    // tile type
    std::string type_name;

    Cell(std::string name);
    Cell(std::string name, Texture* background);
    // change tile texture and name
    void change_texture(std::string name, Texture* texture);
    // change m_vertices
    void addTexCoords(IntRect rect);
    // set position to Cell and all child objects
    void set_position_recursive(double x, double y);
    // is object in object map by name
    bool hasObject(std::string name);
    // add object with name and z-coordinate
    Cell_object* addObject(std::string name, Texture* texture, int depth_level);
    // remove object from map by name
    void removeObject(std::string name);
    // invoke action by name. texture - temporary variable, used for creating <stump>
    void action_change(std::string name, Texture* texture);
    // save cell and child objects to json
    void save_cell(unsigned int x, unsigned int y, Json::Value& Location);

    // overriding Drawable methods
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    void draw_objects(RenderTarget& target, RenderStates states) const;

    // TEMP
    int mapsize() const;
};

#endif // CELL_INCLUDE
