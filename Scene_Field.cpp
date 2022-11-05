#include "Scene_Field.h"

Scene_Field::Scene_Field(std::string name, std::map <std::string, Texture*> *field_blocks) : Scene::Scene(name), field_block(field_blocks)
{
    current_field = -1;
}

void Scene_Field::add_Field(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num)
{
    Field* field_0 = new_field(bg, length, width, (*field_blocks)["B"], player_texture, screen_dimensions);
    field[num] = field_0;
    if (current_field == -1)
    {
        current_field = num;
    }
}

void Scene_Field::change_current_field(int num)
{
    current_field = num;
    field[num]->someTextures(field_block, num);
}

void Scene_Field::update(Event& event)
{
    if (event.type == sf::Event::KeyPressed)
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
            field[current_field]->action((*field_block)["stump"]);
            break;
        case sf::Keyboard::Tab:
            change_current_field((current_field+1)%2);
            break;
        default:
            break;
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

void Scene_Field::update(Time deltaTime)
{
    // empty
}

void Scene_Field::someTextures(int num)
{
    //std::cout << "draw another map" << std::endl;
    field[num]->someTextures(field_block, num);
}

void Scene_Field::draw(RenderTarget& target, RenderStates states) const
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
}

Scene_Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks,
                            Texture* player_texture, Vector2i screen_dimensions, int num)
{
    Scene_Field field_scene(std::string("field_scene"), field_blocks); // FIX (pointer)
    field_scene.add_Field(bg, length, width, field_blocks, player_texture, screen_dimensions, num);
    return field_scene;
}
