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
        Scene_editor(std::string name, sf::Vector2i screensize, std::map <std::string, Texture*> *field_tex_map);

        // overriding Drawable methods
        virtual void update(Event& event, std::string& command_main);
        virtual void update(Time deltaTime);
        // we don't want to override it... but Tima's loading fonts there
        virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif  // SCENE_EDITOR_H_INCLUDED
