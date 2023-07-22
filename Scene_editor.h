#ifndef SCENE_EDITOR_H_INCLUDED
#define SCENE_EDITOR_H_INCLUDED

#include <string>
#include <cassert>

#include "Field.h"
#include "Scene_Field.h"

#include <SFML/Graphics/Text.hpp>

class Scene_editor : public Scene_Field
{
    private:
        // command line string
        std::string s_input;
        // activated command line input flag
        bool input_focus;
        // evaluate command line command
        void command(std::string data);

        // save field to json
        void save_map();

    public:
        Scene_editor(std::string name, std::map <std::string, Texture*> *field_tex_map);

        // overriding Drawable methods
        virtual void update(Event& event, std::string& command_main);
        virtual void update(Time deltaTime);
        // we don't want to override it... but Tima's loading fonts there
        virtual void draw(RenderTarget& target, RenderStates states) const override;
};

/// TEMP
// MyFirstSceneEditor constructor
Scene_editor new_editor_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num);
#endif // SCENE_EDITOR_H_INCLUDED
