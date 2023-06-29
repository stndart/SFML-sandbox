#ifndef FIELD_INCLUDE
#define FIELD_INCLUDE

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "json/json.h"
#include <cassert>

#include "AnimatedSprite.h"
#include "Cell.h"
#include "Player.h"
#include "extra_algorithms.h"

using namespace sf;

class Field : public Drawable, public Transformable
{
private:
    std::vector<std::vector<Cell*> > cells;
    Texture* background;
    Vertex m_vertices[4];
    double cell_0_screen_x, cell_0_screen_y;
    double cell_length_x, cell_length_y;
    unsigned int cell_center_x, cell_center_y;
    View current_view;
    bool view_changed, cells_changed;

    Vector2i default_player_pos; // default player position on field


    Vector2f check_view_bounds(Vector2f view_center);


public:
    std::string name;
    Player* player_0;

    bool field_changed;

    Field(int length, int width, std::string name);
    Field(int length, int width, std::string name, Texture* bg_texture, Vector2i screenDimensions);
    void addTexture(Texture* texture, IntRect rect);
    void addCell(Texture* texture, unsigned int x, unsigned int y);
    void addPlayer(Texture* player_texture, Vector2i pos); // pos default = (-1, -1)

    void field_resize(unsigned int length, unsigned int width);         // CHECK
    // return cell_type (name of the cell)
    std::string get_cellType_by_coord(unsigned int x, unsigned int y);
    bool is_player_movable(int direction);
    void move_player(int direction);
    void set_player_movement_direction(int direction);
    void release_player_movement_direction(int direction);
    std::pair <int, int> get_player_cell_coord(); // WHY?
    void action(Texture* texture);
    void add_object_to_cell(int cell_x, int cell_y, std::string type_name, Texture* texture);
    void change_cell_texture(int cell_x, int cell_y, std::string name, Texture* texture);
    void load_field(std::map <std::string, Texture*> &field_block, int loc_id);
    void save_field(int loc_id);
    void place_characters();
    void update_view_center();

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
