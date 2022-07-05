#include "Scene_Field.h"

Scene_Field::Scene_Field(std::string name, std::map <std::string, Texture*> *field_blocks) : Scene::Scene(name), field_block(field_blocks)
{

}

void Scene_Field::add_Field(Field* field_0)
{
    field = field_0;
}

void Scene_Field::update(Event& event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::W:
            field->move_player(0, 1);
            break;
        case sf::Keyboard::D:
            field->move_player(1, 1);
            break;
        case sf::Keyboard::S:
            field->move_player(0, -1);
            break;
        case sf::Keyboard::A:
            field->move_player(1, -1);
            break;
        case sf::Keyboard::Space:
            field->action_change((*field_block)["S"]);
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

void Scene_Field::someTextures()
{
    field->someTextures(field_block);
}

void Scene_Field::draw(RenderTarget& target, RenderStates states) const
{
    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }
    field->draw(target, states);
}

Scene_Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, std::map <std::string, Texture*> *field_blocks, Texture* player_texture, Vector2i screen_dimensions)
{
    Scene_Field field_scene(std::string("field_scene"), field_blocks); // FIX (pointer)
    Field* field_0 = new_field(bg, length, width, (*field_blocks)["B"], player_texture, screen_dimensions);
    field_scene.add_Field(field_0);
    return field_scene;
}
