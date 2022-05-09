#include "Field.h"
#include <iostream>

Field::Field(int length, int width, std::string name) : background(NULL), name(name)
{
    x_coord = length/2 * 120 + 60;
    y_coord = width/2 * 120 + 60;
    turn_length = 1;
    cell_length = 120;
    //std::cout << x_coord << " " << y_coord << std::endl;
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
    cells[x][y] = new_cell(texture, "new_cell");
}

void Field::addPlayer(unsigned int length, unsigned int width, Texture* player_texture)
{
    player_0 = new Player("default_player", player_texture);
    player_0->x_coord = x_coord;
    player_0->y_coord = y_coord;
    player_0->addTexCoords(IntRect(0, 0, 120, 120));
}

void Field::field_resize(unsigned int length, unsigned int width)         // CHECK
{
    cells.resize(length);
    for (unsigned int i = 0; i < length; i++)
    {
        cells[i].resize(width);
    }
}

void Field::move_player(int direction, int value)
{
    if (direction == 0)
    {
        if ((y_coord - value*turn_length >= 1080/2) && (y_coord - value*turn_length <= cells[0].size()*cell_length-turn_length-1080/2) && player_0->y_coord == y_coord)
        {
            y_coord -= value*turn_length;
        }
        if (player_0->y_coord - value*turn_length >= cell_length && player_0->y_coord - value*turn_length <= (cells[0].size()-1)*cell_length)
        {
            player_0->y_coord -= value*turn_length;
        }
        else return;
    }
    else if (direction == 1)
    {
        if ((x_coord + value*turn_length >= 1920/2) && (x_coord + value*turn_length <= cells.size()*cell_length-turn_length-1920/2) && player_0->x_coord == x_coord)
        {
            x_coord += value*turn_length;
        }
        if (player_0->x_coord + value*turn_length >= cell_length && player_0->x_coord + value*turn_length <= (cells[0].size()-1)*cell_length)
        {
            player_0->x_coord += value*turn_length;
        }
        else return;
    }
    //std::cout << x_coord << " " << y_coord << "\n";
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

    /*for (unsigned int i = 0; i < 16; i++)
    {
        for (unsigned int j = 0; j < 9; j++)
        {
            unsigned int x = player_0->x_coord-8+i;
            unsigned int y = player_0->y_coord-5+j;

            cells[x][y]->setPosition(120*i, 120*j);
            cells[x][y]->draw(target, states);
        }
    }*/
    //cells[20][20]->setPosition(-60, -60);
    //cells[20][20]->draw(target, states);
    for (unsigned int i = 0; i < 17; i++)
    {
        for (unsigned int j = 0; j < 10; j++)
        {
            int x = (x_coord-960)/cell_length;
            int y = (y_coord-540)/cell_length;
            cells[x+i][y+j]->setPosition(x*cell_length-x_coord+960+cell_length*i, y*cell_length-y_coord+540+cell_length*j);
            cells[x+i][y+j]->draw(target, states);
            //std::cout << x*120-x_coord+960+120*i << " " << y*120-y_coord+540+120*j << std::endl;
        }
    }
    if (true)
    {
        player_0->setPosition(900-(x_coord-player_0->x_coord), 480-(y_coord-player_0->y_coord));
        //std::cout << player_0->getPosition().x << " " << player_0->getPosition().y << std::endl;
        player_0->draw(target, states);
    }
}

void Field::someTextures(Texture* texture1, Texture* texture2)
{
    cells[1][1] = new_cell(texture1, "border");
    for (unsigned int x = 0; x < cells[0].size(); x++)
    {
        cells[0][x] = new_cell(texture1, "border");
        cells[cells.size()-1][x] = new_cell(texture1, "border");
    }
    for (unsigned int x = 0; x < cells.size(); x++)
    {
        cells[x][0] = new_cell(texture1, "border");
        cells[x][cells[x].size()-1] = new_cell(texture1, "border");
    }
    for (unsigned int x = 0; x < cells.size(); x++)
    {
        for (unsigned int y = 0; y < cells[x].size(); y++)
        {
            if ((x+1)%8 == 0 && (y+1)%8 == 0)
            {
                cells[x][y] = new_cell(texture2, "grass_8");
            }
        }
    }
}

Field* new_field(Texture* bg, unsigned int length, unsigned int width, Texture* cell_texture, Texture* player_texture, Vector2i screen_dimensions)
{
    Field* field = new Field(length, width, "Main field");
    field->addTexture(bg, IntRect(0, 0, 1920, 1080));
    field->setScale((float)screen_dimensions.x / 1920, (float)screen_dimensions.y / 1080);

    field->field_resize(length, width);

    for (unsigned int x = 0; x < length; x++)
    {
        for (unsigned int y = 0; y < width; y++)
        {

            field->addCell(cell_texture, x, y);
        }
    }

    field->addPlayer(length, width, player_texture);

    return field;
}
