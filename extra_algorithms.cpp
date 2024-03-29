#include "extra_algorithms.h"

#include "Scene_Field.h"

#include "SceneController.h"

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

// gets coordinate-like absolute and relative properties from json and constructs a vector
sf::Vector2f get_coords_from_json(nlohmann::json j, sf::Vector2i windowsize)
{
    sf::Vector2f coords(0, 0);

    // i didn't know .value<float>(key, 0) exists
    if (j.contains("Abs x"))
        coords.x += j["Abs x"].get<float>();
    if (j.contains("Abs y"))
        coords.y += j["Abs y"].get<float>();
    if (j.contains("Rel x"))
        coords.x += j["Rel x"].get<float>() * windowsize.x;
    if (j.contains("Rel y"))
        coords.y += j["Rel y"].get<float>() * windowsize.y;
    
    return coords;
}

sf::Vector2f get_size_from_json(nlohmann::json j, sf::Vector2i windowsize)
{
    sf::Vector2f size(0, 0);

    if (j.contains("Abs width"))
        size.x += j["Abs width"].get<float>();
    if (j.contains("Abs height"))
        size.y += j["Abs height"].get<float>();
    if (j.contains("Rel width"))
        size.x += j["Rel width"].get<float>() * windowsize.x;
    if (j.contains("Rel height"))
        size.y += j["Rel height"].get<float>() * windowsize.y;

    return size;
}
sf::Vector2f get_origin_from_json(nlohmann::json j, sf::Vector2f texsize)
{
    sf::Vector2f origin;

    if (j.contains("Origin abs x"))
        origin.x += j["Origin abs x"].get<float>();
    if (j.contains("Origin abs y"))
        origin.x += j["Origin abs y"].get<float>();
    if (j.contains("Origin rel x"))
        origin.x += j["Origin rel x"].get<float>() * texsize.x;
    if (j.contains("Origin rel y"))
        origin.x += j["Origin rel y"].get<float>() * texsize.y;

    return origin;
}

sf::Vector2f save_aspect_ratio(sf::Vector2f new_size, sf::Vector2f old_size)
{
    sf::Vector2f aspect_ratio_2f = sf::Vector2f(new_size.x / old_size.x, new_size.y / old_size.y);

    // if new size is not specified (0, 0), then stick with old size
    // if only one component is not specified (x, 0), then save aspect ratio
    if (aspect_ratio_2f.x == 0)
        aspect_ratio_2f.x = (aspect_ratio_2f.y == 0) ? 1 : aspect_ratio_2f.y;
    if (aspect_ratio_2f.y == 0)
        aspect_ratio_2f.y = aspect_ratio_2f.x;
    
    float aspect_ratio = std::min(aspect_ratio_2f.x, aspect_ratio_2f.y);

    return old_size * aspect_ratio;
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

