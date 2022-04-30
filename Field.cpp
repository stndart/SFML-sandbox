#include "Field.h"
#include <iostream>

Field::Field(std::string name) : name(name)
{

}

void Field::addTexture(Texture* texture, IntRect rect)
{
    background = texture;

    m_vertices[0].position = Vector2f(0.f, 0.f);
    m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
    m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
    m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

    float left = static_cast<float>(rect.left) + 0.0001f;
    float right = left + static_cast<float>(rect.width);
    float top = static_cast<float>(rect.top);
    float bottom = top + static_cast<float>(rect.height);

    m_vertices[0].texCoords = Vector2f(left, top);
    m_vertices[1].texCoords = Vector2f(left, bottom);
    m_vertices[2].texCoords = Vector2f(right, bottom);
    m_vertices[3].texCoords = Vector2f(right, top);
}

void Field::addCell(Texture* texture, unsigned int x, unsigned int y)
{
    Cell* new_cell = new Cell("new_cell", texture);
    cells[x][y] = new_cell;
}

void Field::addPlayer(unsigned int length, unsigned int width, Texture* player_text)
{
    player.first = (length+1)/2;
    player.second = (width+1)/2;
    player_texture = player_text;
}

void Field::field_resize(unsigned int length, unsigned int width)         // CHECK
{
    cells.resize(length);
    for (unsigned int i = 0; i < length; i++)
    {
        cells[i].resize(width);
    }
}

void Field::update(Event& event)
{
    if (event.type == Event::MouseButtonPressed){
        switch (event.mouseButton.button)
        {
        case Mouse::Left:

            break;

        default:
            break;
        }
    }
}

void Field::update(Time deltaTime)
{

}

void Field::draw(RenderTarget& target, RenderStates states) const
{
    //std::cout << "Who asked " << name << " to draw?\n";
    //std::cout << name << " draw pos " << getPosition().x << " " << getPosition().y << std::endl;

    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }

    cells[42][45]->addCoords(IntRect(0, 0, 120, 120));
    cells[42][45]->draw(target, states);
    cells[42][46]->addCoords(IntRect(0, 120, 120, 240));
    cells[42][46]->draw(target, states);

    /*for (unsigned int i = 0; i < 16; i++)
    {
        for (unsigned int j = 0; j < 9; j++)
        {
            unsigned int x = player.first-8+i;
            unsigned int y = player.second-5+j;

            std::cout << x << " " << y << " | " << i << " " << j << std::endl;
            cells[x][y]->addCoords(IntRect(120*i, 120*j, 120*(i+1), 120*(j+1)));
            cells[x][y]->draw(target, states);
        }
        //return;
    }//*/
    std::cout << std::endl;
}

Field new_field_scene(Texture* bg, unsigned int length, unsigned int width, Texture* cell_texture, Texture* player_texture, Vector2i screen_dimensions)
{
    Field field("Main field");
    field.addTexture(bg, IntRect(0, 0, 1920, 1080));
    field.setScale((float)screen_dimensions.x / 1920, (float)screen_dimensions.y / 1080);

    field.field_resize(length, width);

    for (unsigned int x = 0; x < length; x++)
    {
        for (unsigned int y = 0; y < width; y++)
        {
            field.addCell(cell_texture, x, y);
        }
    }

    field.addPlayer(length, width, player_texture);

    return field;
}
