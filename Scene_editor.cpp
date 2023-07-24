#include "Scene_editor.h"

Scene_editor::Scene_editor(std::string name, sf::Vector2i screensize, std::map <std::string, Texture*> *field_blocks) : Scene_Field(name, screensize, field_blocks),
s_input(""), input_focus(false)
{
    // None
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

    if (current_field != -1)
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
                    x = field[current_field]->player_0->x_cell_coord;
                    y = field[current_field]->player_0->y_cell_coord;
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
                if (field_tex_map->find(s[4]) == field_tex_map->end())
                {
                    s_input = "can't find the texture";
                    return;
                }
                field[current_field]->add_object_to_cell(x, y, s[4], (*field_tex_map)[s[4]]);
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
                    x = field[current_field]->player_0->x_cell_coord;
                    y = field[current_field]->player_0->y_cell_coord;
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
                if (field_tex_map->find(s[4]) == field_tex_map->end())
                {
                    s_input = "can't find the texture";
                    return;
                }
                field[current_field]->change_cell_texture(x, y, s[4], (*field_tex_map)[s[4]]);
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

    for (unsigned int i = 0; i < field_N; i++)
    {
        field[i]->save_field(i);
    }
}

void Scene_editor::update(Event& event, std::string& command_main)
{
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
    // if keyboard press and command line deactivated
    if (event.type == sf::Event::KeyPressed && !input_focus)
    {
        if (current_field != -1)
        {
            // <WASD> - move player. <Space> - turn tree into stump, <tab> - change field
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                if (!controls_blocked)
                    field[current_field]->set_player_movement_direction(3);
                //field[current_field]->move_player(3);
                break;
            case sf::Keyboard::D:
                if (!controls_blocked)
                    field[current_field]->set_player_movement_direction(0);
                //field[current_field]->move_player(0);
                break;
            case sf::Keyboard::S:
                if (!controls_blocked)
                    field[current_field]->set_player_movement_direction(1);
                //field[current_field]->move_player(1);
                break;
            case sf::Keyboard::A:
                if (!controls_blocked)
                    field[current_field]->set_player_movement_direction(2);
                //field[current_field]->move_player(2);
                break;
            case sf::Keyboard::Space:
                if (!controls_blocked)
                    field[current_field]->action((*field_tex_map)["stump"]);
                break;
            case sf::Keyboard::Tab:
                if (!controls_blocked)
                    change_current_field((current_field+1)%2);
                break;
            default:
                break;
            }
        }
    }
    else if (event.type == sf::Event::KeyReleased && !input_focus)
    {
        if (current_field != -1)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                if (!controls_blocked)
                    field[current_field]->release_player_movement_direction(3);
                break;
            case sf::Keyboard::D:
                if (!controls_blocked)
                    field[current_field]->release_player_movement_direction(0);
                break;
            case sf::Keyboard::S:
                if (!controls_blocked)
                    field[current_field]->release_player_movement_direction(1);
                break;
            case sf::Keyboard::A:
                if (!controls_blocked)
                    field[current_field]->release_player_movement_direction(2);
                break;
            default:
                break;
            }
        }
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
            if (UI_update_mouse(curPos, event, command_main))
                return;
            Interface->push_click(curPos);
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
            if (UI_update_mouse(curPos, event, command_main))
                return;
            Interface->release_click(curPos);
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
    пишет в левом верхнем углу текущий тип клетки

    int a = field[current_field]->player_0->x_cell_coord;
    int b = field[current_field]->player_0->y_cell_coord;
    s_input = field[current_field]->get_cellType_by_coord(a, b);
**/
}

void Scene_editor::draw(RenderTarget& target, RenderStates states) const
{
    Scene_Field::draw(target, states);

    if (s_input != "")
    {
        Font font;
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        {
            loading_logger->warn("Failed to load arial font");
        }
        Text text;
        text.setFont(font);
        text.setString(s_input);
        text.setCharacterSize(24);
        text.setStyle(sf::Text::Bold);
        text.setFillColor(sf::Color::Magenta);
        text.setPosition(0, 0);
        target.draw(text);
    }
}
