#include "Scene_Field.h"

#include "Inventory.h"
#include "InventoryDraw.h"

int Scene_Field::FIELD_Z_INDEX = 0;

Scene_Field::Scene_Field(std::string name, sf::Vector2u screensize, std::shared_ptr<ResourceLoader> resload) : Scene(name, screensize, resload)
{
    loading_logger = spdlog::get("loading");
    map_events_logger = spdlog::get("map_events");

    current_field = -1;
}

// returns type name ("Scene_Field" for this class)
std::string Scene_Field::get_type()
{
    return "Scene_Field";
}

// returns constructed subwindow of desired type
std::shared_ptr<UI_window> Scene_Field::subwindow_oftype(std::string name, std::string type)
{
    IntRect UIFrame(0, 0, screensize.x, screensize.y);
    if (type == "inventory draw")
    {
        std::shared_ptr<Player> player = fields[current_field]->player_0;
        if (player)
        {
            Inventory* bag = &player->getCharacter().bag;
            // return std::make_shared<InventoryDraw>(player->name + " " + name, UIFrame, bag, this, resource_manager, true);
            return InventoryDraw::fromInventory(player->name + " " + name, bag, this, resource_manager, true);
        }
    }
    return Scene::subwindow_oftype(name, type);
}

// returns config object to be saved externally
nlohmann::json Scene_Field::get_config()
{
    nlohmann::json j;
    return j;
}

// loads interface and other info from config
void Scene_Field::load_config(std::string config_path)
{
    Scene::load_config(config_path);

    ifstream f(config_path);
    nlohmann::json j = nlohmann::json::parse(f);
    
    // create fields
    for (nlohmann::json j2 : j["fields"])
    {
        int id = j2.at("id");
        int loc_id = j2.at("location id");

        std::string field_name = "field #" + std::to_string(id);
        std::shared_ptr<Field> new_field = std::make_shared<Field>(field_name, screensize, resource_manager);
        new_field->load_field(loc_id);
        add_field(new_field, id);
    }
}

// set field by index. If no field is current, set current to index
void Scene_Field::add_field(std::shared_ptr<Field> field_to_add, int num)
{
    loading_logger->trace("Add field #{} to scene", num);

    while (fields.size() <= num)
        fields.push_back(std::shared_ptr<Field>(nullptr));
    
    fields[num] = field_to_add;

    if (current_field == -1)
        change_current_field(num);
}

// swap to field by index. If index is -1, then switch cyclically
void Scene_Field::change_current_field(int num)
{
    if (num == -1)
        num = (current_field + 1) % fields.size();

    map_events_logger->trace("Changing current field to {}", num);

    current_field = num;

    if (!fields[num]) {
        loading_logger->error("Trying to load null field");
        throw;
    }

    fields[num]->teleport_to();
}

// get field pointer
std::shared_ptr<Field> Scene_Field::get_field(int num) const
{
    return fields[num];
}

/// TEMP
// reload field by index from default file
void Scene_Field::load_field(int num, std::string who_call)
{
    fields[num]->load_field(num);
}

FloatRect Scene_Field::getPlayerLocalBounds() const
{
    FloatRect bounds = FloatRect(0, 0, 0, 0);
    if (current_field != -1 && fields[current_field])
    {
        bounds = fields[current_field]->player_0->getLocalBounds();
        bounds.left -= fields[current_field]->getViewport().left;
        bounds.top -= fields[current_field]->getViewport().top;
    }
    return bounds;
}

FloatRect Scene_Field::getPlayerGlobalBounds() const
{
    FloatRect bounds = FloatRect(0, 0, 0, 0);
    if (current_field != -1 && fields[current_field])
    {
        bounds = fields[current_field]->player_0->getGlobalBounds();
        // doesn't work, when view is boundary-blocked
        // if (field[current_field]->player_0->is_moving())
        // {
        //     bounds.left = (float)field[current_field]->player_0->x_cell_coord * field[current_field]->getCellSize().x;
        //     bounds.top = (float)field[current_field]->player_0->y_cell_coord * field[current_field]->getCellSize().y;
        // }
        bounds.left -= fields[current_field]->getViewport().left;
        bounds.top -= fields[current_field]->getViewport().top;
    }
    return bounds;
}

// blocks keyboard input
void Scene_Field::block_controls(bool blocked)
{
    map_events_logger->debug("Blocked controls {}", blocked);

    controls_blocked = blocked;
    if (blocked)
    {
        for (int i = 0; i < 4; ++i)
            release_player_movement_direction(i);
    }
}

// transfer to Field methods
void Scene_Field::set_player_movement_direction(int direction)
{
    if (current_field != -1 && fields[current_field])
    {
        fields[current_field]->set_player_movement_direction(direction);
    }
}

void Scene_Field::release_player_movement_direction(int direction)
{
    if (current_field != -1 && fields[current_field])
    {
        fields[current_field]->release_player_movement_direction(direction);
    }
}

void Scene_Field::update(Event& event, std::string& command_main)
{
    if (current_field != -1 && fields[current_field])
    {
        if (event.type == sf::Event::KeyPressed)
        {
            // <WASD> - move player. <Space> - turn tree into stump, <tab> - change field
            /// MAGIC NUMBERS: set controls in config file
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                if (!controls_blocked)
                    fields[current_field]->set_player_movement_direction(3);
                break;
            case sf::Keyboard::D:
                if (!controls_blocked)
                    fields[current_field]->set_player_movement_direction(0);
                break;
            case sf::Keyboard::S:
                if (!controls_blocked)
                    fields[current_field]->set_player_movement_direction(1);
                break;
            case sf::Keyboard::A:
                if (!controls_blocked)
                    fields[current_field]->set_player_movement_direction(2);
                break;
            case sf::Keyboard::Space:
                fields[current_field]->action(resource_manager->getObjectTexture("stump"));
                break;
            default:
                // if key is not set in contols, check dynamic bindings
                if (!controls_blocked)
                    evaluate_bound_callbacks(event.key.code);
                break;
            }
        }
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                if (!controls_blocked)
                    fields[current_field]->release_player_movement_direction(3);
                break;
            case sf::Keyboard::D:
                if (!controls_blocked)
                    fields[current_field]->release_player_movement_direction(0);
                break;
            case sf::Keyboard::S:
                if (!controls_blocked)
                    fields[current_field]->release_player_movement_direction(1);
                break;
            case sf::Keyboard::A:
                if (!controls_blocked)
                    fields[current_field]->release_player_movement_direction(2);
                break;
            default:
                break;
            }
        }
    }
}

void Scene_Field::update(Time deltaTime)
{
    Scene::update(deltaTime);

    if (current_field != -1 && fields[current_field])
    {
        fields[current_field]->update(deltaTime);
    }
}

// clears and sorts all drawables by z-index
void Scene_Field::sort_drawables()
{
    Scene::sort_drawables();

    if (current_field != -1 && fields[current_field])
    {
        fields[current_field]->draw_to_zmap_with_view(views, sorted_drawables[FIELD_Z_INDEX]);
    }
}