#include "Field.h"

bool isdrawed = false;

Field::Field(int length, int width, std::string name) : background(NULL), name(name)
{
    current_view = View(Vector2f(400, 200), Vector2f(1920, 1080));
    cells_changed = false;

    cell_center_x = 0;
    cell_center_y = 0;

    cell_length_x = 120;
    cell_length_y = 120;

    cells.resize(length);
    for (int i = 0; i < length; ++i)
    {
        cells[i].resize(width);
    }
}

Field::Field(int length, int width, std::string name, Texture* bg_texture, Vector2i screenDimensions) : Field(length, width, name)
{
    addTexture(bg_texture, IntRect(0, 0, 1920, 1080));
    setScale((float)screenDimensions.x / 1920, (float)screenDimensions.y / 1080);
}

// returns view center with field boundaries check
/// screen resolution is temporary only 1920x1080
Vector2f Field::check_view_bounds(Vector2f view_center)
{
    double view_center_x = view_center.x;
    double view_center_y = view_center.y;

    int size_x = cells.size();
    int size_y = 0;
    if (size_x > 0)
        size_y = cells[0].size();

    // if distance to right border is less than half screen
    if ((size_x - 1) * cell_length_x - view_center_x < 960)
        view_center_x = ((size_x - 1) - 960 / cell_length_x) * cell_length_x;
    // if distance to left border is less than half screen
    if (-1 * cell_length_x + view_center_x < 960)
        view_center_x = (1 + 960 / cell_length_x) * cell_length_x;

    // if distance to bottom border is less than half screen
    if ((size_y - 1) * cell_length_y - view_center_y < 540)
        view_center_y = ((size_y - 1) - 540 / cell_length_y) * cell_length_y;
    // if distance to upper border is less than half screen
    if (-1 * cell_length_y + view_center_y < 540)
        view_center_y = (1 + 540 / cell_length_y) * cell_length_y;

    return Vector2f(view_center_x, view_center_y);
}

// update background with texture, View size with rect, m_vertices with rect as well
void Field::addTexture(Texture* texture, IntRect rect)
{
    background = texture;

    m_vertices[0].position = Vector2f(0.f, 0.f);
    m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
    m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
    m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

    float left = static_cast<float>(rect.left) + 0.0001f;
    float right = left + static_cast<float>(rect.width);
    float top = static_cast<float>(rect.top);
    float bottom = top + static_cast<float>(rect.height);

    m_vertices[0].texCoords = Vector2f(left, top);
    m_vertices[1].texCoords = Vector2f(left, bottom);
    m_vertices[2].texCoords = Vector2f(right, bottom);
    m_vertices[3].texCoords = Vector2f(right, top);

    current_view.setSize(rect.width, rect.height);
}

// add cell by indexes [x, y] with texture
void Field::addCell(Texture* texture, unsigned int x, unsigned int y)
{
    cells[x][y] = new Cell("new_cell", texture);
    cells_changed = true;
}

// create player at cell [pos.x, pos.y] with texture
void Field::addPlayer(Texture* player_texture, Vector2i pos = Vector2i(-1, -1))
{
    player_0 = new Player("default_player", player_texture, IntRect(120, 0, 120, 120));

    // create default animation with some magic constants (to be resolved with addition of resources manager)
    Animation* idle_animation = new Animation();
    idle_animation->addSpriteSheet(*player_texture);
    idle_animation->addFrame(IntRect(0, 0, 120, 120), 0);
    idle_animation->addFrame(IntRect(120, 0, 120, 120), 0);
    player_0->add_animation("idle_animation", idle_animation);
    player_0->set_idle_animation("idle_animation");

    if (pos.x == -1)
    {
        player_0->x_cell_coord = default_player_pos.x;
        player_0->y_cell_coord = default_player_pos.y;
    }
    else
    {
        player_0->x_cell_coord = pos.x;
        player_0->y_cell_coord = pos.y;
    }
}

// change field size and reshape cells 2d vector
/// как оно работает если размер уменьшить - я хз
void Field::field_resize(unsigned int length, unsigned int width)         // CHECK
{
    cells.resize(length);
    for (unsigned int i = 0; i < length; i++)
    {
        cells[i].resize(width);
    }
    cells_changed = true;
}

// return cell_type (name of the cell)
std::string Field::get_cellType_by_coord(unsigned int x, unsigned int y)
{
    return cells[x][y]->type_name;
}

// check player obstacles in direction
bool Field::is_player_movable(int direction)
{
    int cell_x = player_0->x_cell_coord;
    int cell_y = player_0->y_cell_coord;

    if (cells[cell_x][cell_y]->type_name == "border")
        return false;  // Player is stuck

    cell_x += direction_x[direction];
    cell_y += direction_y[direction];
    if (cells[cell_x][cell_y]->type_name == "border")
        return false;  // Next cell is border

    return true;
}

// move player by on cell in direction
void Field::move_player(int direction)
{
    // If player is unmovable - don't move
    if (!is_player_movable(direction))
        return;

    // If player is moving - don't give new movement
    if (player_0->is_moving())
        return;

    // Instantly change cells_coords
    // player screen coords are updated via movement
    player_0->x_cell_coord += direction_x[direction];
    player_0->y_cell_coord += direction_y[direction];

    double movement_x = direction_x[direction] * cell_length_x;
    double movement_y = direction_y[direction] * cell_length_y;

    player_0->move_player(Vector2f(movement_x, movement_y), direction);
}

// schedule player movement in direction
void Field::set_player_movement_direction(int direction)
{
    /// TODO: add cell coords

    double movement_x = direction_x[direction] * cell_length_x;
    double movement_y = direction_y[direction] * cell_length_y;

    player_0->add_movement_direction(Vector2f(movement_x, movement_y), direction);
}

// cancel scheduled movement. If direction doesn't match - do nothing. If there are few directions scheduled - pop only coinciding
void Field::release_player_movement_direction(int direction)
{
    /// TODO: add cell coords
    player_0->release_movement_direction(direction);
}

// invoke an action on cell where player stands, with texture (temp)
// currently changes <tree> to <stump>
void Field::action(Texture* texture)
{
    int cell_x = player_0->x_cell_coord;
    int cell_y = player_0->y_cell_coord;
    Cell* target_cell = cells[cell_x][cell_y];

    if (target_cell->hasObject("tree"))
    {
        target_cell->action_change("tree", texture);
    }
    else if (target_cell->hasObject("stump"))
    {
        target_cell->action_change("stump", texture);
    }
    else if (target_cell->hasObject("portal"))
    {

    }

    cells_changed = true;
}

// adds placeable object to cell by coords, name and with texture
void Field::add_object_to_cell(int cell_x, int cell_y, std::string type_name, Texture* texture)
{
    cells[cell_x][cell_y]->addObject(type_name, texture, 1);
    cells_changed = true;
}

// change cell by coordinates tile name and texture
void Field::change_cell_texture(int cell_x, int cell_y, std::string name, Texture* texture)
{
    cells[cell_x][cell_y]->change_texture(name, texture);
    cells_changed = true;
}

// update all movables screen coordinates as well as View
void Field::place_characters()
{
    if (player_0)
    {
        double player_screen_x, player_screen_y;
        player_screen_x = player_0->x_cell_coord * cell_length_x;
        player_screen_y = player_0->y_cell_coord * cell_length_y;

        if (player_0->movement_animation)
        {
            player_screen_x += player_0->x_cell_movement_coord;
            player_screen_y += player_0->y_cell_movement_coord;
        }
        player_0->setPosition(Vector2f(player_screen_x, player_screen_y));
    }

    update_view_center();
}

// updates view center with player position. If no player, then cell_center_xy
void Field::update_view_center()
{
    Vector2f view_center = Vector2f(cell_center_x * cell_length_x,
                                    cell_center_y * cell_length_y);
    if (player_0)
    {
        view_center = player_0->getPosition();
    }
    view_center = check_view_bounds(view_center);

    current_view.setCenter(view_center);
}

// load field and cells from json file <Locations/loc_%loc_id%>
// field_block provides textures for cells by names (instead of resources manager)
void Field::load_field(std::map <std::string, Texture*> &field_block, int loc_id)
{
    // construct and cout path string
    std::string path = "Locations/loc_";
    path += std::to_string(loc_id);
    path += ".json";
    std::cout << "load_field " << path << std::endl;
    Json::Value Location;
    std::ifstream ifs(path);
    ifs >> Location;

    // get field size
    unsigned int field_length = int(Location["scale"][0].asInt());
    unsigned int field_width = int(Location["scale"][1].asInt());
    default_player_pos.x = Location["default_player_pos"][0].asInt();
    default_player_pos.y = Location["default_player_pos"][1].asInt();
    field_resize(field_length, field_width);

    for (unsigned int x = 0; x < cells.size(); x++)
    {
        for (unsigned int y = 0; y < cells[x].size(); y++)
        {
            // add cell
            std::string cell_type = Location["map"][x][y]["type"].asString();
            cells[x][y] = new Cell(cell_type, field_block[cell_type]);

            // add placeable objects to cell
            std::string object_type = "";
            if (Location["big_objects"][x][y].isArray())
            {
                unsigned int cell_object_size = Location["big_objects"][x][y].size();
                for (unsigned int i = 0; i < cell_object_size; i++)
                {
                    if (Location["big_objects"][x][y][i].isObject())
                    {
                        std::string object_type = Location["big_objects"][x][y][0]["type"].asString();
                        std::string object_depth_level = Location["big_objects"][x][y][0]["depth_level"].asString();
                        cells[x][y]->addObject(object_type, field_block[object_type], 1);
                    }
                }
            }
        }
    }
    std::cout << "loc OK" << std::endl;
    ifs.close();
    cells_changed = true;
}

// save field and cells in json file
void Field::save_field(int loc_id)
{
    std::string path = "Locations/loc_";
    path += std::to_string(loc_id);
    path += ".json";

    Json::Value Location;
    Location["default_player_pos"][0] = 10;
    Location["default_player_pos"][1] = 10;
    Location["scale"][0] = cells.size();
    Location["scale"][1] = cells[0].size();
    for (unsigned int x = 0; x < cells.size(); x++)
    {
        for (unsigned int y = 0; y < cells[x].size(); y++)
        {
            // objects and cell are saved recursively
            cells[x][y]->save_cell(x, y, Location);
        }
    }
    std::ofstream ofs(path);
    ofs << Location;
    ofs.close();
}

void Field::update(Time deltaTime)
{
    // update cell texture and child objects coords
    if (cells_changed)
    {
        double cell_screen_x, cell_screen_y;
        for (unsigned int i = 0; i < cells.size(); ++i)
            for (unsigned int j = 0; j < cells[i].size(); ++j)
            {
                cell_screen_x = i * cell_length_x;
                cell_screen_y = j * cell_length_y;
                cells[i][j]->set_position_recursive(cell_screen_x, cell_screen_y);
            }

        cells_changed = false;
    }

    if (player_0)
    {
        if (player_0->queued_movement_direction.size() > 0)
        {
            // check scheduled moves blocking
            for (auto next_mov = player_0->queued_movement_direction.begin();
                next_mov != player_0->queued_movement_direction.end(); next_mov++)
            {
                next_mov->blocking_checked = true;
                next_mov->blocked = true;
                if (is_player_movable(next_mov->direction))
                {
                    next_mov->blocked = false;
                }
            }
        }

        player_0->update(deltaTime);
    }

    update_view_center();
}

void Field::draw(RenderTarget& target, RenderStates states) const
{
    // save previous view not to destroy UI etc.
    View previous_view = target.getView();
    // we only draw in field View
    target.setView(current_view);

    // draw background below everything
    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }

    // center cell in view
    int center_cell_x = current_view.getCenter().x / cell_length_x;
    int center_cell_y = current_view.getCenter().y / cell_length_y;
    /// Что за магические 5 и 8?
    /// Это (1080/120) / 2 и (1920/120) / 2, т.е. центр.

    // not to draw anything "behind the scenes"
    for (int i = center_cell_x - 9; i < center_cell_x + 9; ++i)
    {
        for (int j = center_cell_y - 6; j < center_cell_y + 6; ++j)
        {
            // borders check
            if (i < 0 || i >= (int)cells.size())
                continue;
            if (j < 0 || j >= (int)cells[i].size())
                continue;

            cells[i][j]->draw(target, states);
        }
    }
    for (int i = center_cell_x - 9; i < center_cell_x + 9; ++i)
    {
        for (int j = center_cell_y - 6; j < center_cell_y + 6; ++j)
        {
            // borders check
            if (i < 0 || i >= (int)cells.size())
                continue;
            if (j < 0 || j >= (int)cells[i].size())
                continue;

            cells[i][j]->draw_objects(target, states);
        }
    }

    // draw player atop of everything
    if (player_0)
    {
        player_0->draw(target, states);
    }

    // restore saved View
    target.setView(previous_view);
}

// MyFirstFieldConstructor (to be removed)
Field* new_field(Texture* bg, unsigned int cell_length, unsigned int cell_width, Texture* cell_texture, Texture* player_texture, Vector2i screen_dimensions)
{
    Field* field = new Field(cell_length, cell_width, "Main field");
    field->addTexture(bg, IntRect(0, 0, 1920, 1080));
    field->setScale((float)screen_dimensions.x / 1920, (float)screen_dimensions.y / 1080);

    field->field_resize(cell_length, cell_width);

    for (unsigned int x = 0; x < cell_length; x++)
    {
        for (unsigned int y = 0; y < cell_width; y++)
        {

            field->addCell(cell_texture, x, y);
        }
    }

    field->addPlayer(player_texture, Vector2i(cell_length / 2, cell_width / 2));
    field->place_characters();

    return field;
}
