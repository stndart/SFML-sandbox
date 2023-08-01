#ifndef SCENE_FIELD_H_INCLUDED
#define SCENE_FIELD_H_INCLUDED

#include <map>
#include <memory>

#include "Scene.h"
#include "Field.h"
#include "ResourceLoader.h"

class Scene_Field : public Scene
{
    static int FIELD_Z_INDEX;

protected:
    // list of maps
    std::vector<std::shared_ptr<Field> > fields;
    // current map index
    int current_field;

    std::shared_ptr<spdlog::logger> map_events_logger;

public:
    std::string name;

    Scene_Field(std::string name, sf::Vector2u screensize, std::shared_ptr<ResourceLoader> resload);

    // returns type name ("Scene_Field" for this class)
    std::string get_type() override;

    // returns config object to be saved externally
    nlohmann::json get_config() override;
    // loads interface and other info from config
    void load_config(std::string config_path) override;

    // set field by index. If no field is current, set current to index
    void add_field(std::shared_ptr<Field> field_to_add, int num);
    // swap to field by index. If index is -1, then switch cyclically
    void change_current_field(int num=-1);
    // get field pointer
    std::shared_ptr<Field> get_field(int num) const;

    /// TEMP
    // reload field by index from default file
    void load_field(int num, std::string who_call);
    /// NOT IMPLEMENTED
    void save_map();

    FloatRect getPlayerLocalBounds() const;
    FloatRect getPlayerGlobalBounds() const;

    void block_controls(bool blocked);

    // transfer to Field methods
    void set_player_movement_direction(int direction);
    void release_player_movement_direction(int direction);

    // overriding Drawable methods
    void update(Event& event, std::string& command_main) override;
    void update(Time deltaTime) override;
    // we don't override draw since it stays the same

    ///TEMP
    int mapsize(int x, int y)
    {
        return fields[current_field]->mapsize(x, y);
    }
};

#endif // SCENE_FIELD_H_INCLUDED
