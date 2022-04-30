#ifndef FIELD_INCLUDE
#define FIELD_INCLUDE

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "AnimatedSprite.h"
#include "Cell.h"

using namespace sf;

class Field : public Drawable, public Transformable
{
private:
    std::vector<std::vector<Cell*> > cells;
    Texture* background;
    Vertex m_vertices[4];
    void draw(RenderTarget& target, RenderStates states) const override;
    std::pair<int, int> player;
    Texture* player_texture;
public:
    std::string name;
    Field(std::string name);
    void addTexture(Texture* texture, IntRect rect);
    void addCell(Texture* texture, unsigned int x, unsigned int y);
    void addPlayer(unsigned int length, unsigned int width, Texture* player_texture);
    void field_resize(unsigned int length, unsigned int width);         // CHECK
    virtual void update(Event& event);
    virtual void update(Time deltaTime);
};

Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, Texture* cell_texture, Texture* player_texture, Vector2i screen_dimensions);

#endif // FIELD_INCLUDE
