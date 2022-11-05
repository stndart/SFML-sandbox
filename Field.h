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
    Player* player_0;
    double x_coord;
    double y_coord;
    int turn_length;
    int cell_length;
public:
    std::string name;
    Field(int length, int width, std::string name);
    void addTexture(Texture* texture, IntRect rect);
    void addCell(Texture* texture, unsigned int x, unsigned int y);
    void addPlayer(unsigned int length, unsigned int width, Texture* player_texture);
    void field_resize(unsigned int length, unsigned int width);         // CHECK
    //Cell* get_cell_by_coord(unsigned int x, unsigned int y);
    bool can_move_player(int direction, int value);
    void move_player(int direction, int value);
    std::pair <int, int> get_player_cell_coord();
    void action(Texture* texture);
    void add_object_to_cell(std::string type_name, int x, int y, Texture* texture);
    void change_cell_texture(std::string name, int x, int y, Texture* texture);
    void someTextures(std::map <std::string, Texture*> *field_block, int num);
    void save_field(int num);
    void draw(RenderTarget& target, RenderStates states) const override; //not in private

    //TEMP
    int mapsize(int x, int y)
    {
        return cells[x][y]->mapsize();
    }
};

Field* new_field(Texture* bg, unsigned int length, unsigned int width, Texture* cell_texture, Texture* player_texture, Vector2i screen_dimensions);

#endif // FIELD_INCLUDE
