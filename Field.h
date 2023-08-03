#ifndef FIELD_INCLUDE
#define FIELD_INCLUDE

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <algorithm>

#include <cassert>

#include "AnimatedSprite.h"
#include "Cell.h"
#include "Player.h"
#include "Item.h"
#include "extra_algorithms.h"
#include "ResourceLoader.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

using namespace sf;

class Field : public Drawable, public Transformable
{
    static int CELL_Z_INDEX;

private:
    // to draw background
    sf::Sprite background;

    // 2d vector of cells
    std::vector<std::vector<std::shared_ptr<Cell> > > cells;

    // size of field onscreen. Used to update view position
    Vector2u field_screen_size;
    // cell texture size
    Vector2u cell_tex_size;
    // cell size in pixels
    double cell_length_x, cell_length_y;
    // index of center cell (usually that under player)
    unsigned int cell_center_x, cell_center_y;

    // Rectangle, which cuts a window of 1920x1080 (or other resolution) in a huuuuge field. i.e. shows what to display
    View current_view;
    // default player position on field
    Vector2i default_player_pos;
    // flag if any objects in cells or cell texture changed
    bool cells_changed;

    // returns view center with field boundaries check
    Vector2f correct_view_bounds(Vector2f view_center);

    // FloatRect containing all possible view center coords
    // If view center is located outside this rect, field borders become visible
    // has_border: if field has border, that should not be displayed
    FloatRect get_valid_view_center_rect(bool has_border = true);

    std::shared_ptr<ResourceLoader> resource_manager;

    std::shared_ptr<spdlog::logger> map_events_logger, loading_logger;

public:
    // field name
    std::string name;
    // player that is controlled by user. It is the only player. Controls are bound to it
    std::shared_ptr<Player> player_0;

    Field(std::string name, Vector2u screenDimensions, std::shared_ptr<ResourceLoader> resload);
    Field(std::string name, std::shared_ptr<Texture> bg_texture, Vector2u screenDimensions, std::shared_ptr<ResourceLoader> resload);

    // update background with texture, View size with rect, and texture rect with rect as well
    void addTexture(std::shared_ptr<Texture> texture, IntRect rect);
    // add cell by indexes [x, y] with texture
    void addCell(std::shared_ptr<Texture> texture, unsigned int x, unsigned int y);

    // change field size and reshape cells 2d vector
    void field_resize(unsigned int length, unsigned int width);
    // return cell_type (name of the cell)
    std::string get_cellType_by_coord(unsigned int x, unsigned int y);

    // add player at cell [cell_x, cell_y]
    void addPlayer(std::shared_ptr<Player> player, Vector2i pos = Vector2i(-1, -1));

    // places player onto this field by coords and changes its scale to fit into cell
    void teleport_to(std::shared_ptr<Player> player = std::shared_ptr<Player>(nullptr));
    void teleport_to(Vector2i coords, std::shared_ptr<Player> player = std::shared_ptr<Player>(nullptr));

    // check player obstacles in direction
    bool is_player_movable(int direction);
    // move player by on cell in direction
    void move_player(int direction);
    // schedule player movement in direction
    void set_player_movement_direction(int direction);
    // cancel scheduled movement. If direction doesn't match - do nothing. If there are few directions scheduled - pop only coinciding
    void release_player_movement_direction(int direction);

    // invoke an action on cell where player stands, with texture (temp)
    // currently changes <tree> to <stump>
    // <stump> to nothing
    // jumps through portal to move by (-3, +10) from it
    void action(std::shared_ptr<Texture> texture);
    // adds placeable object to cell by coords, name and with texture
    void add_object_to_cell(int cell_x, int cell_y, std::string type_name, std::shared_ptr<Texture> texture, int z_level = 1);
    // change cell by coordinates tile name and texture
    void change_cell_texture(int cell_x, int cell_y, std::string name, std::shared_ptr<Texture> texture);

    // update all movables screen coordinates as well as View
    void place_characters();
    // updates view center with player position. If no player, then cell_center_xy
    void update_view_center();
    // get view rectangle
    FloatRect getViewport() const;
    // get Cell size on screen
    Vector2f getCellSize() const;

    // load field and cells from json file <Locations/loc_%loc_id%>
    void load_field(int loc_id);
    // save field and cells in json file
    void save_field(int loc_id);

    // overriding Transformable methods
    virtual void move(const Vector2f &offset);
    virtual void rotate(float angle);
    virtual void scale(const Vector2f &factor);
    virtual void setPosition(const Vector2f &position);
    virtual void setPosition(float x, float y);
    virtual void setScale(const Vector2f &factors);
    virtual void setScale(float factorX, float factorY);

    // overriding Drawable methods
    virtual void update(Time deltaTime);
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    // before drawing send itself to sort by z-index
    // first index is view, second is z-index
    virtual void draw_to_zmap_with_view(std::vector<View> &views, std::map<int, std::map<int, std::vector<const Drawable*> > > &zmap) const;

    ///TEMP
    int mapsize(int x, int y)
    {
        return cells[x][y]->mapsize();
    }
};

#endif // FIELD_INCLUDE
