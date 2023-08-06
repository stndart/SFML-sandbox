#include "Scene_editor.h"

Scene_editor::Scene_editor(std::string name, sf::Vector2u screensize, std::shared_ptr<ResourceLoader> resload) : Scene_Field(name, screensize, resload),
s_input(""), input_focus(false)
{
    // None
}

// returns type name ("Scene_editor" for this class)
std::string Scene_editor::get_type()
{
    return "Scene_editor";
}

// evaluate command line command
void Scene_editor::command(std::string data)
{
    map_events_logger->info("Scene_editor running command \"{}\"", data);

    // split string <data> by whitespaces
    std::vector <std::string> s(1);
    for (unsigned int i = 0; i < data.size(); i++)
    {
        if (data[i] != ' ')
        {
            s[s.size()-1] += data[i];
        }
        else
        {
            s.push_back("");
        }
    }

    // switch by first command word

    if (current_field != -1 && fields[current_field])
    {
        if (s[0] == "save_map")
        {
            save_map();
        }
        else if (s[0] == "add")
        {
            // "add" "object" [x] [y] [object name]
            if (s.size() < 5)
            {
                s_input = "not enough arguments";
                return;
            }
            if (s[1] == "object")
            {
                int x = -1;
                int y = -1;
                if (s[2] == "~" && s[3] == "~")
                {
                    x = fields[current_field]->player_0->x_cell_coord;
                    y = fields[current_field]->player_0->y_cell_coord;
                }
                else
                {
                    x = stoi(s[2]);
                    y = stoi(s[3]);
                }
                if (x <= 0 || y <= 0)
                {
                    s_input = "out of field's range";
                    return;
                }
                fields[current_field]->add_object_to_cell(x, y, s[4], resource_manager->getObjectTexture(s[4]));
            }
            else
            {
                s_input = "can't find an object";
                return;
            }
        }
        else if (s[0] == "change")
        {
            // "change" "cell" [x] [y] [new tile name]
            if (s.size() < 5)
            {
                s_input = "not enough arguments";
                return;
            }
            if (s[1] == "cell")
            {
                int x = -1;
                int y = -1;
                if (s[2] == "~" && s[3] == "~")
                {
                    x = fields[current_field]->player_0->x_cell_coord;
                    y = fields[current_field]->player_0->y_cell_coord;
                }
                else
                {
                    x = stoi(s[2]);
                    y = stoi(s[3]);
                }
                if (x <= 0 || y <= 0)
                {
                    s_input = "out of field's range";
                    return;
                }
                fields[current_field]->change_cell_texture(x, y, s[4], resource_manager->getCellTexture(s[4]));
            }
            else
            {
                s_input = "can't find this";
                return;
            }
        }
        else
        {
            s_input = "Invalid command";
            return;
        }
    }
}

void Scene_editor::save_map()
{
    loading_logger->info("Saving scene_editor map");

    for (unsigned int i = 0; i < fields.size(); i++)
    {
        fields[i]->save_field(i);
    }
}

void Scene_editor::update(Event& event)
{
    Scene_Field::update(event);
    
    // if <Enter> then mess with command line
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
    {
        input_logger->debug("Pressed enter with current string \"{}\"", s_input);

        // save old content
        std::string data = s_input;
        s_input = "";
        // if smth present then run command
        if (data != "")
        {
            input_logger->info("running command: \"{}\"", data);
            command(data);
        }
        input_focus = !input_focus;
    }
    // if command line active and pressed: <backspace>
    else if (input_focus && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace)
    {
        /// ??? what are these asserts for
        //assert(s_input.size() == 6);
        if (s_input.size() > 0) s_input.pop_back();
        //assert(s_input.size() == 5);
    }
    // if command line active and pressed any letter
    else if (input_focus && event.type == sf::Event::TextEntered)
    {
        input_logger->trace("Logging pressed symbol: \"{}\"", event.text.unicode);

        // ignore special symbols
        if (event.text.unicode != '\r' && event.text.unicode != '\b' && event.text.unicode != '\n')
        {
            char letter = event.text.unicode;
            s_input += letter;
        }
    }
    // pressing buttons
    if (event.type == Event::MouseButtonPressed)
    {
        Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            /// WHY?
            if (UI_update_mouse(curPos, event))
                return;
            Interface->push_click(curPos, controls_blocked);
            break;

        default:
            break;
        }
    }
    // unpressing buttons
    if (event.type == Event::MouseButtonReleased)
    {
        Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            /// WHY?
            if (UI_update_mouse(curPos, event))
                return;
            Interface->release_click(curPos, controls_blocked);
            break;

        default:
            break;
        }
    }
}

void Scene_editor::update(Time deltaTime)
{
    Scene_Field::update(deltaTime);

/**
    int a = fields[current_field]->player_0->x_cell_coord;
    int b = fields[current_field]->player_0->y_cell_coord;
    s_input = fields[current_field]->get_cellType_by_coord(a, b);
**/
}

void Scene_editor::draw(RenderTarget& target, RenderStates states) const
{
    Scene_Field::draw(target, states);

    if (s_input != "")
    {
        std::shared_ptr<Font> font = resource_manager->getFont("Arial");
        Text text;
        text.setFont(*font);
        text.setString(s_input);
        text.setCharacterSize(24);
        text.setStyle(sf::Text::Bold);
        text.setFillColor(sf::Color::Magenta);
        text.setPosition(0, 0);
        target.draw(text);
    }
}