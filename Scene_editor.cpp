#include "Scene_editor.h"

Scene_editor::Scene_editor(std::string name, std::map <std::string, Texture*> *field_blocks):Scene_Field(name, field_blocks)
{
    if_input = false;
    s_input = "";
}

void Scene_editor::update(Event& event, std::string& command_main)
{
    //std::cout << event.key.code << " ";
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
    {
        std::string data = s_input;
        s_input = "";
        if (data != "")
        {
            std::cout << data << std::endl;
            command(data);
        }
        if_input = !if_input;
    }
    if (event.type == sf::Event::KeyPressed && !if_input)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::W:
            field[current_field]->move_player(3);
            break;
        case sf::Keyboard::D:
            field[current_field]->move_player(0);
            break;
        case sf::Keyboard::S:
            field[current_field]->move_player(1);
            break;
        case sf::Keyboard::A:
            field[current_field]->move_player(2);
            break;
        case sf::Keyboard::Space:
            field[current_field]->action((*field_block)["stump"]);
            break;
        case sf::Keyboard::Tab:
            change_current_field((current_field+1)%2);
            break;
        default:
            break;
        }
    }
    else if (if_input && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace)
    {
        /*int cg = '\b';
        std::cout << cg << " " << s_input.size() << std::endl;
        if (s_input.size() == 6)
        {
            for (unsigned int i = 0; i < 6; i++)
            {
                int ch = s_input[i];
                std::cout << "|" << ch << "|";
            }
            std::cout << std::endl;
            for (unsigned int i = 0; i < 6; i++)
            {
                char ch = s_input[i];
                std::cout << "|" << ch << "|";
            }
            std::cout << std::endl;
        }*/
        //assert(s_input.size() == 6);
        if (s_input.size() > 0) s_input.pop_back();
        //assert(s_input.size() == 5);
        //std::cout << s_input << std::endl;
    }
    else if (if_input && event.type == sf::Event::TextEntered)
    {
        if (event.text.unicode != '\r' && event.text.unicode != '\b' && event.text.unicode != '\n')
        {
            char letter = event.text.unicode;
            s_input += letter;
        }
    }
    if (event.type == Event::MouseButtonPressed){
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            for (auto b : buttons)
            {
                Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);
                if (b->contains(curPos))
                {
                    b->push_button();
                    pushed_buttons.push_back(b);
                    break;
                }
            }
            break;

        default:
            break;
        }
    }
    if (event.type == Event::MouseButtonReleased)
    {
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            while (pushed_buttons.size() > 0)
            {
                Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);
                std::string answer = pushed_buttons[pushed_buttons.size()-1]->release_button();
                if (pushed_buttons[pushed_buttons.size()-1]->contains(curPos))
                {
                    command_main = answer;
                }
                pushed_buttons.pop_back();
            }
            break;

        default:
            break;
        }
    }
}

void Scene_editor::update(Time deltaTime)
{
    field[current_field]->update(deltaTime);
}

void Scene_editor::command(std::string data)
{
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
    /*for (unsigned int i = 0; i < s.size(); i++)
    {
        std::cout << s[i] << std::endl;
    }*/
    if (s[0] == "save_map")
    {
        save_map();
    }
    else if (s[0] == "add")
    {
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
            if (field_block->find(s[4]) == field_block->end())
            {
                s_input = "can't find the texture";
                return;
            }
            field[current_field]->add_object_to_cell(x, y, s[4], (*field_block)[s[4]]);
        }
        else
        {
            s_input = "can't find an object";
            return;
        }
    }
    else if (s[0] == "change")
    {
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
            if (field_block->find(s[4]) == field_block->end())
            {
                s_input = "can't find the texture";
                return;
            }
            field[current_field]->change_cell_texture(x, y, s[4], (*field_block)[s[4]]);
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

void Scene_editor::save_map()
{
    for (unsigned int i = 0; i < field_size; i++)
    {
        field[i]->save_field(i);
    }
}
void Scene_editor::draw(RenderTarget& target, RenderStates states) const
{
    /*if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }*/
    if (current_field != -1)
    {
        field[current_field]->draw(target, states);
    }
    if (s_input != "")
    {
        Font font;
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        {
            std::cout << "failed to load arial\n";
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
    draw_scene_buttons(target, states);
}

Scene_editor new_editor_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num)
{
    Scene_editor editor_scene(std::string("editor_scene"), field_blocks); // FIX (pointer)
    editor_scene.add_Field(bg, length, width, field_blocks, player_texture, screen_dimensions, num);
    return editor_scene;
}

