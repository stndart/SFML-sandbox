#include "Scene_editor.h"

Scene_editor::Scene_editor(std::string name, std::map <std::string, Texture*> *field_blocks):Scene_Field(name, field_blocks)
{
    if_input = false;
    s_input = "";
}

void Scene_editor::command(std::string data)
{

}

void Scene_editor::update(Event& event)
{
    //std::cout << event.key.code << " ";
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
    {
        if (s_input != "")
        {
            std::cout << s_input << std::endl;
        }
        else
        {
            s_input = "";
        }
        if_input = !if_input;
    }
    if (event.type == sf::Event::KeyPressed && !if_input)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::W:
            field[current_field]->move_player(0, 1);
            break;
        case sf::Keyboard::D:
            field[current_field]->move_player(1, 1);
            break;
        case sf::Keyboard::S:
            field[current_field]->move_player(0, -1);
            break;
        case sf::Keyboard::A:
            field[current_field]->move_player(1, -1);
            break;
        case sf::Keyboard::Space:
            field[current_field]->action((*field_block)["S"]);
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
        s_input.pop_back();
    }
    else if (if_input && event.type == sf::Event::TextEntered)
    {
        if (event.text.unicode != '\n')
        {
            char letter = event.text.unicode;
            s_input += letter;
        }
    }
    /*if (event.type == Event::MouseButtonReleased) // ?
    {
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            break;

        default:
            break;
        }
    }*/
}

void Scene_editor::update(Time deltaTime)
{
    // empty
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
        sf::Text text;
        text.setString("Hello world");
        text.setCharacterSize(24);
        text.setStyle(sf::Text::Bold);
        text.setFillColor(sf::Color::Magenta);
        target.draw(text);
    }
}

Scene_editor new_editor_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num)
{
    Scene_editor editor_scene(std::string("editor_scene"), field_blocks); // FIX (pointer)
    editor_scene.add_Field(bg, length, width, field_blocks, player_texture, screen_dimensions, num);
    return editor_scene;
}

