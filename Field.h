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
    // texture corners on background texture
    Vertex m_vertices[4];
    Texture* background;

    // 2d vector of cells
    std::vector<std::vector<Cell*> > cells;

    /// не знаю, что это
    double cell_0_screen_x, cell_0_screen_y;
    // cell size in pixels
    double cell_length_x, cell_length_y;
    /// не знаю, что это
    unsigned int cell_center_x, cell_center_y;

    // Rectangle, which cuts a window of 1920x1080 (or other resolution) in a huuuuge field. i.e. shows what to display
    View current_view;
    // flags if view or cells are to update
    /// не знаю, зачем это
    bool view_changed, cells_changed;

    // returns view center with field boundaries check
    Vector2f check_view_bounds(Vector2f view_center);
public:
    // field name
    std::string name;
    // player object that is controlled by player. It is the only player object. Controls are bound to it
    Player* player_0;

    /// не знаю, зачем это
    bool field_changed;

    Field(int length, int width, std::string name);
    Field(int length, int width, std::string name, Texture* bg_texture, Vector2i screenDimensions);

    // update background with texture, View size with rect, m_vertices with rect as well
    void addTexture(Texture* texture, IntRect rect);
    // add cell by indexes [x, y] with texture
    void addCell(Texture* texture, unsigned int x, unsigned int y);
    // create player at cell [cell_x, cell_y] with texture
    void addPlayer(Texture* player_texture, unsigned int cell_x, unsigned int cell_y);
    // change field size and reshape cells 2d vector
    /// как оно работает если размер уменьшить - я хз
    void field_resize(unsigned int length, unsigned int width);         // CHECK

    // check player obstacles in direction
    bool is_player_movable(int direction);
    // move player by on cell in direction (instantly)
    void move_player(int direction);
    // schedule player smooth movement in direction
    void set_player_movement_direction(int direction);
    // cancel scheduled movement. If direction doesn't match - do nothing. If there are few directions scheduled - pop only coinciding
    void release_player_movement_direction(int direction);

    void action(Texture* texture);
    void add_object_to_cell(int cell_x, int cell_y, std::string type_name, Texture* texture);
    void change_cell_texture(int cell_x, int cell_y, std::string name, Texture* texture);
    void place_characters();
    void update_view_center();

    void load_field(std::map <std::string, Texture*> &field_block, int loc_id);
    void save_field(int loc_id);

    // overriding Drawable methods
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
