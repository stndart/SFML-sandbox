#ifndef FIELD_INCLUDE
#define FIELD_INCLUDE

#include <vector>
#include <map>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <cassert>
#include "AnimatedSprite.h"
#include "Cell.h"
#include "Player.h"

using namespace sf;

class Field : public Drawable, public Transformable
{
private:
    std::vector<std::vector<Cell*> > cells;
    Texture* background;
    Vertex m_vertices[4];
    double cell_0_screen_x, cell_0_screen_y;
    double cell_length_x, cell_length_y;
public:
    std::string name;
    Player* player_0;
    Field(int length, int width, std::string name);
    void addTexture(Texture* texture, IntRect rect);
    void addCell(Texture* texture, unsigned int x, unsigned int y);
    void addPlayer(Texture* player_texture, unsigned int cell_x, unsigned int cell_y);
    void field_resize(unsigned int length, unsigned int width);         // CHECK
    //Cell* get_cell_by_coord(unsigned int x, unsigned int y);
    bool is_player_movable(int direction);
    void move_player(int direction);
    std::pair <int, int> get_player_cell_coord(); // WHY?
    void action(Texture* texture);
    void add_object_to_cell(int cell_x, int cell_y, std::string type_name, Texture* texture);
    void change_cell_texture(int cell_x, int cell_y, std::string name, Texture* texture);
    void someTextures(std::map <std::string, Texture*> &field_block, int num);
    void save_field(int num);

    virtual void draw(RenderTarget& target, RenderStates states) const override;
    virtual void update(Time deltaTime);

    //TEMP
    int mapsize(int x, int y)
    {
        return cells[x][y]->mapsize();
    }
};

Field* new_field(Texture* bg, unsigned int cell_length, unsigned int cell_width,
                 Texture* cell_texture, Texture* player_texture, Vector2i screen_dimensions);

#endif // FIELD_INCLUDE
