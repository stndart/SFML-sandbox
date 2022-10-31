#ifndef CELL_INCLUDE
#define CELL_INCLUDE

#include <map>

#include <SFML/Graphics/Sprite.hpp>
#include "AnimatedSprite.h"
#include "Cell_object.h"

using namespace sf;

class Cell : public Drawable, public Transformable
{
private:
    Vertex m_vertices[4];
    const Texture* background;
    std::map <std::string, Cell_object*> objects;
public:
    std::string name;
    Cell(std::string name);
    Cell(std::string name, Texture* background);
    Cell(std::string name, Texture* background, Texture* texture1);
    void addTexture(Texture* texture);
    void addTexCoords(IntRect rect);
    void addPosition(float x, float y);
    bool hasObject(std::string name);
    void addObject(Texture* texture, std::string name);
    void removeObject(std::string name);
    void action_change(std::string name, Texture* texture);
    void draw(RenderTarget& target, RenderStates states) const override;

    // TEMP
    int mapsize()
    {
        return objects.size();
    }
};

Cell* new_cell(Texture* bg, std::string name);

#endif // CELL_INCLUDE
