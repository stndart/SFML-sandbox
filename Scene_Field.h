#ifndef SCENE_FIELD_H_INCLUDED
#define SCENE_FIELD_H_INCLUDED

#include <map>

#include "Scene.h"
#include "Field.h"

class Scene_Field : public Scene
{
protected:
    Field* field[2];
    int current_field;
    std::map <std::string, Texture*> *field_block;
    void draw(RenderTarget& target, RenderStates states) const override;
public:
    std::string name;
    unsigned int field_size = 2;
    Scene_Field(std::string name, std::map <std::string, Texture*> *field_block);
    void add_Field(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num);
    void change_current_field(int num);
    virtual void update(Event& event);
    virtual void update(Time deltaTime);
    void someTextures(int num);
    void save_map();

    //TEMP
    int mapsize(int x, int y)
    {
        return field[current_field]->mapsize(x, y);
    }
};

Scene_Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num);
#endif // SCENE_FIELD_H_INCLUDED
