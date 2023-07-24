#include "extra_algorithms.h"

#include "Scene.h"
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

std::function<void()> create_change_scene_callback(std::shared_ptr<Scene> scene, std::string scene_to)
{
    std::function<void()> callback = [scene, scene_to]{
        scene->get_scene_controller().set_current_scene(scene_to);
    };
    return callback;
}

std::function<void()> create_window_closed_callback(std::shared_ptr<sf::RenderWindow> window)
{
    std::function<void()> callback = [window] {
        window->close();
    };
    return callback;
}

/// MEMORY LEAK
// creates semi-transparent colored sprite, that covers field
std::function<void()> create_bloodscreen(std::shared_ptr<Scene> scene, const sf::Color& color)
{
    std::function<void()> callback = [scene, color] {
        AnimatedSprite* as = new AnimatedSprite("bloodscreen", std::make_unique<RectangleShape>(Vector2f(1920, 1080)), Vector2f(0, 0));
        State start = {0, Color(0, 0, 0, 0),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        State finish = {0, color,
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        VisualEffect* ve = new VisualEffect(as, seconds(1), seconds(3), start, finish);
        ve->play();
        scene->addSprite(ve);
    };
    return callback;
}

std::function<void()> create_light_circle(std::shared_ptr<Scene> scene, sf::Vector2f pos, float radius, const sf::Color& color)
{
    std::function<void()> callback = [scene, pos, radius, color] {
        AnimatedSprite* as = new AnimatedSprite("light circle", std::make_unique<CircleShape>(radius), pos);
        State start = {0, Color(0, 0, 0, 0),
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        State finish = {0, color,
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        VisualEffect* ve = new VisualEffect(as, seconds(1), seconds(3), start, finish);
        ve->play();
        scene->addSprite(ve);
    };
    return callback;
}

// cleares scene->sprites
std::function<void()> clear_bloodscreen(std::shared_ptr<Scene> scene)
{
    std::function<void()> callback = [scene] {
        scene->delete_sprites();
    };
    return callback;
}

// prepares texture to display: sets vertexes to screen and tex coords
void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect, sf::IntRect texFrame)
{
    // Where to draw texture (relative to sprite origin)
    m_vertices[0].position = Vector2f(0.f, 0.f);
    m_vertices[1].position = Vector2f(0.f, static_cast<float>(rect.height));
    m_vertices[2].position = Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
    m_vertices[3].position = Vector2f(static_cast<float>(rect.width), 0.f);

    float left = static_cast<float>(texFrame.left);/// + 0.0001f;
    float right = left + static_cast<float>(texFrame.width);
    float top = static_cast<float>(texFrame.top);
    float bottom = top + static_cast<float>(texFrame.height);

    // What to draw (coordinate on texture)
    m_vertices[0].texCoords = Vector2f(left, top);
    m_vertices[1].texCoords = Vector2f(left, bottom);
    m_vertices[2].texCoords = Vector2f(right, bottom);
    m_vertices[3].texCoords = Vector2f(right, top);
}

void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect)
{
    cutout_texture_to_frame(m_vertices, rect, rect);
}

int direction_x[4] = {1, 0, -1, 0};
int direction_y[4] = {0, 1, 0, -1};

