#include "Field.h"
#include "Player.h"

Field::Field(std::string name, Vector2u screenDimensions, std::shared_ptr<ResourceLoader> resload) :
    field_screen_size(screenDimensions), resource_manager(resload),
    name(name), player_0(std::shared_ptr<Player>(nullptr))
{
    // Reaching out to global "loading" logger and "map_events" logger by names
    map_events_logger = spdlog::get("map_events");
    loading_logger = spdlog::get("loading");

    /// MAGIC NUMBERS!
    cell_length_x = 120;
    cell_length_y = 120;
    cell_center_x = 0;
    cell_center_y = 0;

    cell_tex_size = Vector2u(120, 120);

    current_view = View(Vector2f(400, 200), Vector2f(1920, 1080));
    default_player_pos = Vector2i(1, 1);
    cells_changed = false;
}

Field::Field(std::string name, std::shared_ptr<Texture> bg_texture, Vector2u screenDimensions, std::shared_ptr<ResourceLoader> resload) : Field(name, screenDimensions, resload)
{
    /// MAGIC NUMBERS!
    addTexture(bg_texture, IntRect(0, 0, 1920, 1080));
    setScale((float)screenDimensions.x / 1920, (float)screenDimensions.y / 1080);
}

// returns view center with field boundaries check
Vector2f Field::correct_view_bounds(Vector2f view_center)
{
    FloatRect valid_rect = get_valid_view_center_rect();

    if (valid_rect.contains(view_center))
        return view_center;
    
    if (valid_rect.left > view_center.x)
        view_center.x = valid_rect.left;
    if (valid_rect.left + valid_rect.width < view_center.x)
        view_center.x = valid_rect.left + valid_rect.width;
    
    if (valid_rect.top > view_center.y)
        view_center.y = valid_rect.top;
    if (valid_rect.top + valid_rect.height < view_center.y)
        view_center.y = valid_rect.top + valid_rect.height;
    
    return view_center;
}

// FloatRect containing all possible view center coords
// If view center is located outside this rect, field borders become visible
// has_border: if field has border, that should not be displayed
FloatRect Field::get_valid_view_center_rect(bool has_border)
{
    Vector2f topleft = Vector2f(field_screen_size) / 2.0f;
    if (has_border)
        topleft += Vector2f(cell_length_x, cell_length_y);

    Vector2f field_full_size(0, 0);
    if (cells.size() > 0 && cells[0].size() > 0)
        field_full_size = Vector2f(cell_length_x * cells.size(), cell_length_y * cells[0].size());
    
    Vector2f downright = field_full_size - Vector2f(field_screen_size) / 2.0f;
    if (has_border)
        downright -= Vector2f(cell_length_x, cell_length_y);
    if (downright.x < topleft.x && downright.y < topleft.y)
        downright = topleft;
    
    FloatRect valid_view_center(topleft, downright - topleft);

    // if displayed field is less than desired field screen size, then move it to screen center
    if ((downright - topleft).x < field_screen_size.x && (downright - topleft).y < field_screen_size.y)
        valid_view_center =  FloatRect(Vector2f(field_full_size) / 2.0f, Vector2f(0, 0));
    
    // map_events_logger->trace("Set valid view center rect to +{}+{}, {}x{}",
    //     valid_view_center.left, valid_view_center.top, valid_view_center.width, valid_view_center.height);
    
    return valid_view_center;
}

// update background with texture, View size with rect, m_vertices with rect as well
void Field::addTexture(std::shared_ptr<Texture> texture, IntRect rect)
{
    background.setTexture(*texture);
    background.setTextureRect(rect);

    current_view.setSize(rect.width, rect.height);
}

// add cell by indexes [x, y] with texture
void Field::addCell(std::shared_ptr<Texture> texture, unsigned int x, unsigned int y)
{
    // if cell_tex_size is (0, 0), then ask certain texture of her size
    Vector2u temp_cell_tex_size = cell_tex_size;
    if (temp_cell_tex_size.x == 0 || temp_cell_tex_size.y == 0)
        temp_cell_tex_size = texture->getSize();

    IntRect cell_tex_coords = IntRect(temp_cell_tex_size.x * x, temp_cell_tex_size.y * y,
                                      temp_cell_tex_size.x, temp_cell_tex_size.y);

    cells[x][y] = std::make_shared<Cell>("new_cell", texture, cell_tex_coords);
    cells_changed = true;
}

// change field size and reshape cells 2d vector
/// как оно работает если размер уменьшить - я хз
void Field::field_resize(unsigned int length, unsigned int width)         // CHECK
{
    map_events_logger->debug("Changed field size to {}x{}", length, width);

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

// create player at cell [pos.x, pos.y] with texture
void Field::addPlayer(std::shared_ptr<Texture> player_texture, Vector2i pos)
{
    if (pos == Vector2i(-1, -1))
        pos = default_player_pos;

    player_0 = std::make_shared<Player>("default_player", player_texture, FloatRect(0, 0, 120, 120));
    player_0->set_current_field(this);

    // create default animation with some magic constants (to be resolved with addition of resources manager)
    std::shared_ptr<Animation> idle_animation = std::make_shared<Animation>();
    idle_animation->addSpriteSheet(player_texture);
    /// MAGIC NUMBERS!
    idle_animation->addFrame(IntRect(0, 0, 120, 120), 0);
    idle_animation->addFrame(IntRect(120, 0, 120, 120), 0);
    player_0->add_animation("idle_animation", idle_animation);

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

// create player at cell [pos.x, pos.y] with animations files: [idle, movement_0]
void Field::addPlayer(std::vector<std::string> animation_filenames, Vector2i pos, Vector2u frame_size)
{
    if (pos == Vector2i(-1, -1))
        pos = default_player_pos;

    std::shared_ptr<Texture> p_tex = std::make_shared<Texture>();
    if (!p_tex->loadFromFile("Images/Flametail/default.png"))
    {
        loading_logger->error("Failed to load texture");
        throw;
    }
    /// TODO: change to no-default-texture-player
    player_0 = std::make_shared<Player>("animated_player", p_tex, FloatRect(0, 0, frame_size.x, frame_size.y));
    player_0->set_current_field(this);
    map_events_logger->trace("Created player");

    // create default animation with some magic constants (to be resolved with addition of resources manager)
    std::shared_ptr<Animation> idle_animation_0 = make_shared<Animation>();
    idle_animation_0->load_from_file(animation_filenames[0], frame_size);
    idle_animation_0->add_joint(-1, "idle_animation_0", 1);

    idle_animation_0->add_joint(5,  "movement_0", 1);
    idle_animation_0->add_joint(10, "movement_0", 1);
    idle_animation_0->add_joint(15, "movement_0", 1);
    idle_animation_0->add_joint(20, "movement_0", 1);
    idle_animation_0->add_joint(25, "movement_0", 1);
    idle_animation_0->add_joint(30, "movement_0", 1);
    idle_animation_0->add_joint(35, "movement_0", 1);

    idle_animation_0->add_joint(5,  "movement_2", 1);
    idle_animation_0->add_joint(10, "movement_2", 1);
    idle_animation_0->add_joint(15, "movement_2", 1);
    idle_animation_0->add_joint(20, "movement_2", 1);
    idle_animation_0->add_joint(25, "movement_2", 1);
    idle_animation_0->add_joint(30, "movement_2", 1);
    idle_animation_0->add_joint(35, "movement_2", 1);

    std::shared_ptr<Animation> idle_animation_2 = make_shared<Animation>();
    idle_animation_2->load_from_file(animation_filenames[1], frame_size);
    idle_animation_2->add_joint(-1, "idle_animation_2", 1);

    idle_animation_2->add_joint(5,  "movement_0", 1);
    idle_animation_2->add_joint(10, "movement_0", 1);
    idle_animation_2->add_joint(15, "movement_0", 1);
    idle_animation_2->add_joint(20, "movement_0", 1);
    idle_animation_2->add_joint(25, "movement_0", 1);
    idle_animation_2->add_joint(30, "movement_0", 1);
    idle_animation_2->add_joint(35, "movement_0", 1);

    idle_animation_2->add_joint(5,  "movement_2", 1);
    idle_animation_2->add_joint(10, "movement_2", 1);
    idle_animation_2->add_joint(15, "movement_2", 1);
    idle_animation_2->add_joint(20, "movement_2", 1);
    idle_animation_2->add_joint(25, "movement_2", 1);
    idle_animation_2->add_joint(30, "movement_2", 1);
    idle_animation_2->add_joint(35, "movement_2", 1);

    std::shared_ptr<Animation> movement_0 = make_shared<Animation>();
    movement_0->load_from_file(animation_filenames[2], frame_size);
    movement_0->add_joint(-1, "movement_0", 1);
    movement_0->add_joint(-1, "idle_animation_0", 1);

    std::shared_ptr<Animation> movement_2 = make_shared<Animation>();
    movement_2->load_from_file(animation_filenames[3], frame_size);
    movement_2->add_joint(-1, "movement_2", 1);
    movement_2->add_joint(-1, "idle_animation_2", 1);

    map_events_logger->trace("Field: created 4 animations, joints joined");

    player_0->add_animation("idle_animation_0", idle_animation_0);
    player_0->add_animation("idle_animation_2", idle_animation_2);
    player_0->add_animation("movement_0", movement_0);
    player_0->add_animation("movement_2", movement_2);

    map_events_logger->trace("Field: added 4 animations to player");

    // fit sprite into cell (horizontally)
    player_0->setScale(Vector2f(cell_length_x / frame_size.x, cell_length_x / frame_size.x));

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

    loading_logger->info("Loaded player at cell {}x{}", player_0->x_cell_coord, player_0->y_cell_coord);
}

// places player onto this field by coords
void Field::teleport_to(std::shared_ptr<Player> player)
{
    map_events_logger->info("Teleporting player to default position on field {}", name);

    Vector2i coords = default_player_pos;
    teleport_to(coords, player);
}

void Field::teleport_to(Vector2i coords, std::shared_ptr<Player> player)
{
    map_events_logger->info("Teleporting player position {}x{} on field {}", coords.x, coords.y, name);

    if (player)
        player_0 = player;

    if (player_0)
    {
        player_0->set_current_field(this);

        player_0->x_cell_coord = coords.x;
        player_0->y_cell_coord = coords.y;

        place_characters();
    }
    else
    {
        map_events_logger->warn("Teleport to {}x{}, but no player passed to {}", coords.x, coords.y, name);
    }
}

// check player obstacles in direction
bool Field::is_player_movable(int direction)
{
    if (player_0 && player_0->get_current_field() == this)
    {
        int cell_x = player_0->x_cell_coord;
        int cell_y = player_0->y_cell_coord;

        if (cells[cell_x][cell_y]->has_out_block(direction))
            return false; // Player is stuck

        cell_x += direction_x[direction];
        cell_y += direction_y[direction];
        if (cells[cell_x][cell_y]->has_in_block(direction))
            return false;  // Next cell is unreachable

        return true;
    }
    // if there is no player, then it is unmovable
    return false;
}

// move player by on cell in direction
void Field::move_player(int direction)
{
    if (player_0 && player_0->get_current_field() == this)
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
}

// schedule player movement in direction
void Field::set_player_movement_direction(int direction)
{
    /// TODO: add cell coords

    if (player_0 && player_0->get_current_field() == this)
    {
        double movement_x = direction_x[direction] * cell_length_x;
        double movement_y = direction_y[direction] * cell_length_y;

        player_0->add_movement_direction(Vector2f(movement_x, movement_y), direction);
    }
}

// cancel scheduled movement. If direction doesn't match - do nothing. If there are few directions scheduled - pop only coinciding
void Field::release_player_movement_direction(int direction)
{
    if (player_0 && player_0->get_current_field() == this)
    {
        /// TODO: add cell coords
        player_0->release_movement_direction(direction);
    }
}

// invoke an action on cell where player stands, with texture (temp)
// currently changes <tree> to <stump>
void Field::action(std::shared_ptr<Texture> texture)
{
    if (player_0 && player_0->get_current_field() == this)
    {
        int cell_x = player_0->x_cell_coord;
        int cell_y = player_0->y_cell_coord;
        map_events_logger->debug("Calling action on cell {}x{}", cell_x, cell_y);

        std::shared_ptr<Cell> target_cell = cells[cell_x][cell_y];
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
}

// adds placeable object to cell by coords, name and with texture
void Field::add_object_to_cell(int cell_x, int cell_y, std::string type_name, std::shared_ptr<Texture> texture)
{
    cells[cell_x][cell_y]->addObject(type_name, texture, 1);
    cells_changed = true;
}

// change cell by coordinates tile name and texture
void Field::change_cell_texture(int cell_x, int cell_y, std::string name, std::shared_ptr<Texture> texture)
{
    cells[cell_x][cell_y]->change_texture(name, texture);
    cells_changed = true;
}

// update all movables screen coordinates as well as View
void Field::place_characters()
{
    map_events_logger->trace("Updating player and view screen coords");

    if (player_0 && player_0->get_current_field() == this)
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

    if (player_0 && player_0->get_current_field() == this)
    {
        view_center = player_0->getPosition();
    }
    view_center = correct_view_bounds(view_center);

    // map_events_logger->trace("new view center: +{}+{}, {}x{}", view_center.x, view_center.y, current_view.getSize().x, current_view.getSize().y);

    current_view.setCenter(view_center);
}

// get view rectangle
FloatRect Field::getViewport() const
{
//    map_events_logger->info("VIEWPORT {}x{}", current_view.getViewport().left, current_view.getViewport().top);
//    map_events_logger->info("VIEWPORT center {}x{}", current_view.getCenter().x, current_view.getCenter().y);
    Vector2f topleft = current_view.getCenter() - Vector2f(current_view.getSize().x / 2.0f, current_view.getSize().y / 2.0f);
    Vector2f getsize = current_view.getSize();
    return FloatRect(topleft.x, topleft.y, getsize.x, getsize.y);
}

// get Cell size
Vector2f Field::getCellSize() const
{
    return Vector2f(cell_length_x, cell_length_y);
}

// load field and cells from json file <Locations/loc_%loc_id%>
// field_block provides textures for cells by names (instead of resources manager)
void Field::load_field(int loc_id)
{
    // construct and log path string
    std::string path = "Locations/loc_";
    path += std::to_string(loc_id);
    path += ".json";

    loading_logger->info("Loading field from \"{}\"", path);

    Json::Value Location;
    std::ifstream ifs(path);
    ifs >> Location;

    // get field size
    unsigned int field_length = int(Location["scale"][0].asInt());
    unsigned int field_width = int(Location["scale"][1].asInt());

    loading_logger->info("Loading field with size of {}x{}", field_length, field_width);

    default_player_pos.x = Location["default_player_pos"][0].asInt();
    default_player_pos.y = Location["default_player_pos"][1].asInt();
    field_resize(field_length, field_width);

    cell_tex_size = Vector2u(Location["cell_texture_size"].get(Json::ArrayIndex(0), 0).asInt(),
                                      Location["cell_texture_size"].get(Json::ArrayIndex(1), 0).asInt());

    for (unsigned int x = 0; x < cells.size(); x++)
    {
        for (unsigned int y = 0; y < cells[x].size(); y++)
        {
            // add cell
            std::string cell_type = Location["map"][x][y]["type"].asString();

            // if cell_tex_size is (0, 0), then ask certain texture of her size
            Vector2u temp_cell_tex_size = cell_tex_size;
            if (temp_cell_tex_size.x == 0 || temp_cell_tex_size.y == 0)
                temp_cell_tex_size = resource_manager->getCellTexture(cell_type)->getSize();

            IntRect cell_tex_coords = IntRect(temp_cell_tex_size.x * x, temp_cell_tex_size.y * y,
                                              temp_cell_tex_size.x, temp_cell_tex_size.y);
            cells[x][y] = std::make_shared<Cell>(cell_type, resource_manager->getCellTexture(cell_type), cell_tex_coords);

            cells[x][y]->displayed = Location["map"][x][y].get("displayed", true).asBool();
            
            // when cell is border-like, enable full movement blocking: in and out
            if (cell_type == "border")
                for (int dir = 0; dir < 4; dir++)
                {
                    cells[x][y]->set_out_block(dir, true);
                    cells[x][y]->set_in_block(dir, true);
                }

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
                        int object_depth_level = Location["big_objects"][x][y][0]["depth_level"].asInt();
                        float displayed_width = Location["big_objects"][x][y][0].get("displayed_width", 0).asFloat();
                        float displayed_height = Location["big_objects"][x][y][0].get("displayed_height", 0).asFloat();
                        Vector2f object_displayed_size = save_aspect_ratio(
                            Vector2f(displayed_width, displayed_height),
                            Vector2f(resource_manager->getObjectTexture(object_type)->getSize())
                        );

                        cells[x][y]->addObject(object_type, resource_manager->getObjectTexture(object_type), object_displayed_size, object_depth_level);
                        
                        float origin_x = Location["big_objects"][x][y][0]["origin"].get(Json::ArrayIndex(0), 0).asFloat();
                        float origin_y = Location["big_objects"][x][y][0]["origin"].get(Json::ArrayIndex(1), 0).asFloat();
                        cells[x][y]->setOrigin(origin_x, origin_y);
                        
                        // load blocking info from json
                        Json::Value in_blocking = Location["big_objects"][x][y][0]["in_blocking"];
                        for (int dir = 0; dir < 4; dir++)
                            cells[x][y]->update_in_block(dir, in_blocking.get(Json::ArrayIndex(dir), false).asBool());
                        Json::Value out_blocking = Location["big_objects"][x][y][0]["out_blocking"];
                        for (int dir = 0; dir < 4; dir++)
                            cells[x][y]->update_out_block(dir, out_blocking.get(Json::ArrayIndex(dir), false).asBool());

                        loading_logger->trace("Added object {} to cell {} at {}x{}", object_type, cell_type, x, y);
                    }
                }
            }
        }
    }
    ifs.close();
    cells_changed = true;
}

// save field and cells in json file
void Field::save_field(int loc_id)
{
    std::string path = "Locations/loc_";
    path += std::to_string(loc_id);
    path += ".json";

    loading_logger->info("Saving field to \"{}\"", path);

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

// overriding Transformable methods
void Field::move(const Vector2f &offset)
{
    Transformable::move(offset);
    background.move(offset);
}

void Field::rotate(float angle)
{
    Transformable::rotate(angle);
    background.rotate(angle);
}

void Field::scale(const Vector2f &factor)
{
    Transformable::scale(factor);
    background.scale(factor);
}

void Field::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    background.setPosition(position);
}

void Field::setPosition(float x, float y)
{
    Transformable::setPosition(x, y);
    background.setPosition(x, y);
}

void Field::setScale(const Vector2f &factors)
{
    Transformable::setScale(factors);
    background.setScale(factors);
}

void Field::setScale(float factorX, float factorY)
{
    Transformable::setScale(factorX, factorY);
    background.setScale(factorX, factorY);
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

    if (player_0 && player_0->get_current_field() == this)
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
    // draw background below everything
    if (background.getTexture())
    {
        target.draw(background, states);
    }
    
    // save previous view not to destroy UI etc.
    View previous_view = target.getView();
    // we only draw in field View
    target.setView(current_view);

    // center cell in view
    int center_cell_x = current_view.getCenter().x / cell_length_x;
    int center_cell_y = current_view.getCenter().y / cell_length_y;

    /// MAGIC NUMBERS!
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

    // draw player atop of everythinghas current field? {}", (player_0->get_current_field() == this));
    if (player_0 && player_0->get_current_field() == this)
    {
        player_0->draw(target, states);
    }

    // restore saved View
    target.setView(previous_view);
}