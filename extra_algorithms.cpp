#include "extra_algorithms.h"


// extract filename (without extension) from full path
std::string re_name(std::string path)
{
    std::string result = "";
    for (unsigned int i = 0; i < path.size(); i++)
    {
        if (path[i] == '/')
        {
            result = "";
            continue;
        }
        else if (path[i] == '.')
        {
            break;
        }
        else result += path[i];
    }
    return result;
}

// get direction from vector2f movement; Direction is enumerate 0, 1, 2, 3: east, south, west, north
int direction_from_shift(sf::Vector2f shift)
{
    if (shift == sf::Vector2f(0, 0))
        return -1; // default undirected direction

    int direction = 0;
    if (abs(shift.x) > abs(shift.y))
    {
        if (shift.x > 0)
            direction = 0;
        else
            direction = 2;
    }
    else
    {
        if (shift.y > 0)
            direction = 1;
        else
            direction = 3;
    }
    return direction;
}

// prepares texture to display: sets vertexes to screen and tex coords
void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect, sf::IntRect texFrame)
{
    // Where to draw texture (relative to sprite origin)
    m_vertices[0].position = sf::Vector2f(0.f, 0.f);
    m_vertices[1].position = sf::Vector2f(0.f, static_cast<float>(rect.height));
    m_vertices[2].position = sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
    m_vertices[3].position = sf::Vector2f(static_cast<float>(rect.width), 0.f);

    float left = static_cast<float>(texFrame.left);/// + 0.0001f;
    float right = left + static_cast<float>(texFrame.width);
    float top = static_cast<float>(texFrame.top);
    float bottom = top + static_cast<float>(texFrame.height);

    // What to draw (coordinate on texture)
    m_vertices[0].texCoords = sf::Vector2f(left, top);
    m_vertices[1].texCoords = sf::Vector2f(left, bottom);
    m_vertices[2].texCoords = sf::Vector2f(right, bottom);
    m_vertices[3].texCoords = sf::Vector2f(right, top);
}

void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect)
{
    cutout_texture_to_frame(m_vertices, rect, rect);
}

int direction_x[4] = {1, 0, -1, 0};
int direction_y[4] = {0, 1, 0, -1};

