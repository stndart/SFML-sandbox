#ifndef SCENE_FIELD_H_INCLUDED
#define SCENE_FIELD_H_INCLUDED

#include "Scene.h"
#include "Field.h"

class Scene_Field : public Scene
{
private:
    Field* field;
    void draw(RenderTarget& target, RenderStates states) const override;
public:
    std::string name;
    Scene_Field(std::string name);
    void add_Field(Field* field_0);
    virtual void update(Event& event);
    virtual void update(Time deltaTime);
    void someTextures(Texture* texture1, Texture* texture2);
};

Scene_Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, Texture* cell_texture, Texture* player_texture, Vector2i screen_dimensions);
#endif // SCENE_FIELD_H_INCLUDED
