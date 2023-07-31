#ifndef SCENE_EDITOR_H_INCLUDED
#define SCENE_EDITOR_H_INCLUDED

#include <string>
#include <cassert>

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
        Scene_editor(std::string name, sf::Vector2u screensize, std::shared_ptr<ResourceLoader> resload);

        // returns type name ("Scene_editor" for this class)
        std::string get_type() override;

        // overriding Drawable methods
        void update(Event& event, std::string& command_main) override;
        void update(Time deltaTime) override;
        void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // SCENE_EDITOR_H_INCLUDED
