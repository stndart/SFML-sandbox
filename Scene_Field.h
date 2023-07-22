#ifndef SCENE_FIELD_H_INCLUDED
#define SCENE_FIELD_H_INCLUDED

#include <map>

#include "Scene.h"
#include "Field.h"

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
    // map background textures by name
    std::map <std::string, Texture*> *field_tex_map;

    std::shared_ptr<spdlog::logger> map_events_logger;

    /// UserInterface UI; // implement UserInterface to do

public:
    std::string name;
    Scene_Field(std::string name, std::map <std::string, Texture*> *field_tex_map);

    // change field by index
    void add_field(Field* field_to_add, int num);
    /// TEMP
    // create field, add by index
    void add_Field(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num);
    // swap to field by index
    void change_current_field(int num);
    /// TEMP
    // reload field by index from default file
    void load_field(int num, std::string who_call);
    /// NOT IMPLEMENTED
    void save_map();

    // transfer to Field methods
    virtual void set_player_movement_direction(int direction);
    virtual void release_player_movement_direction(int direction);

    // overriding Drawable methods
    virtual void update(Event& event, std::string& command_main);
    virtual void update(Time deltaTime);
    // we don't override draw since it stays the same

    ///TEMP
    int mapsize(int x, int y)
    {
        return field[current_field]->mapsize(x, y);
    }
};

Scene_Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num);
#endif // SCENE_FIELD_H_INCLUDED
