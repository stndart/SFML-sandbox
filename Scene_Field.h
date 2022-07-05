#ifndef SCENE_FIELD_H_INCLUDED
#define SCENE_FIELD_H_INCLUDED

#include <map>

#include "Scene.h"
#include "Field.h"

class Scene_Field : public Scene
{
private:
    Field* field;
    std::map <std::string, Texture*> *field_block;
    void draw(RenderTarget& target, RenderStates states) const override;
public:
    std::string name;
    Scene_Field(std::string name, std::map <std::string, Texture*> *field_block);
    void add_Field(Field* field_0);
    virtual void update(Event& event);
    virtual void update(Time deltaTime);
    void someTextures();
};

Scene_Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks, Texture* player_texture, Vector2i screen_dimensions);
#endif // SCENE_FIELD_H_INCLUDED
