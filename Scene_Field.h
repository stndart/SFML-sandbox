#ifndef SCENE_FIELD_H_INCLUDED
#define SCENE_FIELD_H_INCLUDED

#include <map>

#include "Scene.h"
#include "Field.h"
#include "ResourceLoader.h"

class Scene_Field : public Scene
{
    static int FIELD_Z_INDEX;

public:
    // number of maps
    /// MAGIC NUMBER (replace with vector)
    static const int field_N = 2;

protected:
    // list of maps
    Field* field[field_N];
    // current map index
    int current_field;
    // pointer to resource manager
    ResourceLoader* resource_manager;

    // map background textures by name
    std::map <std::string, Texture*> *field_tex_map;

    std::shared_ptr<spdlog::logger> map_events_logger;

public:
    std::string name;

    Scene_Field(std::string name, sf::Vector2u screensize, ResourceLoader* resload);

    // returns type name ("Scene_Field" for this class)
    std::string get_type() override;

    // change field by index
    void add_field(Field* field_to_add, int num);
    /// TEMP
    // create field, add by index
    void add_Field(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2u screen_dimensions, int num);
    // swap to field by index. If index is -1, then switch cyclically
    void change_current_field(int num=-1);
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
        return field[current_field]->mapsize(x, y);
    }
};

Scene_Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2u screen_dimensions, int num);
#endif // SCENE_FIELD_H_INCLUDED
