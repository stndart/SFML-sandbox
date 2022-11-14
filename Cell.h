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
    Vertex m_vertices[4];
    const Texture* background;
    std::map <std::string, Cell_object*> objects;
public:
    std::string type_name;
    Cell(std::string name);
    Cell(std::string name, Texture* background);
    void change_texture(std::string name, Texture* texture);
    void addTexCoords(IntRect rect);
    void set_position_recursive(double x, double y);
    bool hasObject(std::string name);
    Cell_object* addObject(std::string name, Texture* texture, int depth_level);
    void removeObject(std::string name);
    void action_change(std::string name, Texture* texture);
    void save_cell(unsigned int x, unsigned int y, Json::Value& Location);


    void draw(RenderTarget& target, RenderStates states) const override;

    // TEMP
    int mapsize()
    {
        return objects.size();
    }
};

#endif // CELL_INCLUDE
