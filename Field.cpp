#include "Field.h"
#include <iostream>
#include <fstream>
#include <string>

#include "json/json.h"

bool isdrawed = false;

int direction_x[4] = {1, 0, -1, 0};
int direction_y[4] = {0, 1, 0, -1};


Field::Field(int length, int width, std::string name) : background(NULL), name(name)
{
    current_view = View(Vector2f(400, 200), Vector2f(1920, 1080));
    view_changed = true;
    cells_changed = false;

    cell_center_x = 0;
    cell_center_y = 0;

    cell_length_x = 120;
    cell_length_y = 120;
//    cell_0_screen_x = (length + 1) * cell_length_x / 2;
//    cell_0_screen_y = (width  + 1) * cell_length_y / 2;
    cell_0_screen_x = 0;
    cell_0_screen_y = 0;
}

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
    view_changed = true;
}

void Field::addCell(Texture* texture, unsigned int x, unsigned int y)
{
    cells[x][y] = new Cell("new_cell", texture);
    cells_changed = true;
}

void Field::addPlayer(Texture* player_texture, unsigned int cell_x, unsigned int cell_y)
{
    player_0 = new Player("default_player", player_texture, IntRect(120, 0, 120, 120));
    Animation* idle_animation = new Animation();
    idle_animation->addSpriteSheet(*player_texture);
    idle_animation->addFrame(IntRect(0, 0, 120, 120), 0);
    idle_animation->addFrame(IntRect(120, 0, 120, 120), 0);
    player_0->add_animation("idle_animation", idle_animation);
    player_0->set_idle_animation("idle_animation");

    player_0->x_cell_coord = cell_x;
    player_0->y_cell_coord = cell_y;

    update_view_center();
}

void Field::field_resize(unsigned int length, unsigned int width)         // CHECK
{
    cells.resize(length);
    for (unsigned int i = 0; i < length; i++)
    {
        cells[i].resize(width);
    }
    cells_changed = true;
}

bool Field::is_player_movable(int direction)
{
    int cell_x = player_0->x_cell_coord;
    int cell_y = player_0->y_cell_coord;
    if (cells[cell_x][cell_y]->type_name == "border")
        return false;  /// Player is stuck

    cell_x += direction_x[direction];
    cell_y += direction_y[direction];
    if (cells[cell_x][cell_y]->type_name == "border")
        return false;  /// Next cell is border

    return true;
}

/// Change player cell_coords
void Field::move_player(int direction)
{
    /// If player is unmovable - don't move
    if (!is_player_movable(direction))
        return;

    /// If player is moving - don't give new movement
    if (player_0->is_moving())
        return;

    int cell_x = player_0->x_cell_coord;
    int cell_y = player_0->y_cell_coord;
    cell_x += direction_x[direction];
    cell_y += direction_y[direction];

    /// Instantly change cells_coords
    /// Animation is to catch up
    player_0->x_cell_coord = cell_x;
    player_0->y_cell_coord = cell_y;

    double movement_x = direction_x[direction] * cell_length_x;
    double movement_y = direction_y[direction] * cell_length_y;

    player_0->move_player(Vector2f(movement_x, movement_y), direction);
}

void Field::action(Texture* texture)
{
    //std::cout << player_0->x_coord << " " << player_0->y_coord << std::endl;
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

void Field::add_object_to_cell(int cell_x, int cell_y, std::string type_name, Texture* texture)
{
    cells[cell_x][cell_y]->addObject(type_name, texture, 1);
    cells_changed = true;
}

void Field::change_cell_texture(int cell_x, int cell_y, std::string name, Texture* texture)
{
    cells[cell_x][cell_y]->change_texture(name, texture);
    cells_changed = true;
}

/// Спасибо, очень понятная функция, особенно из-за названия и комментариев
void Field::someTextures(std::map <std::string, Texture*> &field_block, int num)
{
    std::string path = "Locations/loc_";
    path += std::to_string(num);
    path += ".json";
    std::cout << path << std::endl;
    Json::Value Locations;
    std::ifstream ifs(path);
    ifs >> Locations;


    for (unsigned int x = 0; x < cells.size(); x++)
    {
        for (unsigned int y = 0; y < cells[x].size(); y++)
        {
            std::string cell_type = Locations["map"][x][y]["type"].asString();
            cells[x][y] = new Cell(cell_type, field_block[cell_type]);

            std::string object_type = "";
            if (Locations["big_objects"][x][y].isArray())
            {
                unsigned int cell_object_size = Locations["big_objects"][x][y].size();
                for (unsigned int i = 0; i < cell_object_size; i++)
                {
                    if (Locations["big_objects"][x][y][i].isObject())
                    {
                        std::string object_type = Locations["big_objects"][x][y][0]["type"].asString();
                        std::string object_depth_level = Locations["big_objects"][x][y][0]["depth_level"].asString();
                        cells[x][y]->addObject(object_type, field_block[object_type], 1);
                    }
                }
            }
        }
    }
    std::cout << std::endl;
}

/// Saves field to "/Locations/loc_%d.json"
void Field::save_field(int num)
{
    std::string path = "Locations/loc_";
    path += std::to_string(num);
    path += ".json";

    Json::Value Location;
    for (unsigned int x = 0; x < cells.size(); x++)
    {
        for (unsigned int y = 0; y < cells[x].size(); y++)
        {
            cells[x][y]->save_cell(x, y, Location);
        }
    }
    std::ofstream ofs(path);
    ofs << Location;
    ofs.close();
}

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

    //for (auto c : characters)
    update_view_center();
}

void Field::update_view_center()
{
    double view_center_x = cell_center_x * cell_length_x;
    double view_center_y = cell_center_y * cell_length_y;
    if (player_0)
    {
        view_center_x = player_0->getPosition().x;
        view_center_y = player_0->getPosition().y;
    }
    current_view.setCenter(Vector2f(view_center_x, view_center_y));
    view_changed = false;
}

void Field::update(Time deltaTime)
{
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
        cell_0_screen_x = player_0->x_cell_coord * cell_length_x;
        cell_0_screen_y = player_0->y_cell_coord * cell_length_y;
        if (cell_center_x != player_0->x_cell_coord || cell_center_y != player_0->y_cell_coord)
        {
            cell_center_x = player_0->x_cell_coord;
            cell_center_y = player_0->y_cell_coord;
        }

        player_0->update(deltaTime);
        view_changed = true;

    }

    if (view_changed)
        update_view_center();
}

void Field::draw(RenderTarget& target, RenderStates states) const
{
    target.setView(current_view);

    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }

    int center_cell_x = cell_0_screen_x / cell_length_x;
    int center_cell_y = cell_0_screen_y / cell_length_y;
    /// Что за магические 5 и 8? Я знаю, что это 16/2 и 10/2, а 10 и 16 - что такое?
    for (int i = center_cell_x - 8; i < center_cell_x + 9; ++i)
        for (int j = center_cell_y - 5; j < center_cell_y + 5; ++j)
        {
            // borders check
            if (i < 0 || i >= (int)cells.size())
                continue;
            if (j < 0 || j >= (int)cells[i].size())
                continue;

            cells[i][j]->draw(target, states);
        }

    if (player_0)
    {
        player_0->draw(target, states);
    }
}

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

    field->addPlayer(player_texture, cell_length / 2, cell_width / 2);
    field->place_characters();

    return field;
}
