#include "Scene.h"

Scene::Scene(std::string name) : background(NULL), name(name)
{
    loading_logger = spdlog::get("loading");
    input_logger = spdlog::get("input");

    Interface = new UI_window("Interface", IntRect(0, 0, 1920, 1080));
}

// change texture and update background from it
void Scene::addTexture(Texture* texture, IntRect rect)
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

void Scene::addSprite(AnimatedSprite* sprite)
{
    loading_logger->debug("Added sprite \"{}\" to scene", sprite->name);

    sprites.push_back(sprite);
}

// add and place button
void Scene::addButton(std::string name, Texture* texture_default, Texture* texture_released, IntRect pos_frame)
{
    loading_logger->debug("Added button \"{}\" to scene", name);

    IntRect tex_frame(0, 0, 0, 0);
    Vector2u tex_size = texture_default->getSize();
    tex_frame.width = tex_size.x;
    tex_frame.height = tex_size.y;

    /// MEMORY LEAK
    Animation* tAn = new Animation;
    tAn->addSpriteSheet(texture_default);
    tAn->addFrame(tex_frame, 0);
    tAn->addSpriteSheet(texture_released);
    tAn->addFrame(tex_frame, 1);

    loading_logger->warn("Created new Animation for button: MEMORY LEAK");

    UI_button* new_button = new UI_button(name, pos_frame, tAn, true);
    Interface->addElement(new_button);
}

void Scene::addButton(std::string name, Texture* texture_default, Texture* texture_released, int pos_x, int pos_y)
{
    IntRect pos_frame(pos_x, pos_y, 0, 0);
    Vector2u tex_size = texture_default->getSize();
    pos_frame.width = tex_size.x;
    pos_frame.height = tex_size.y;

    addButton(name, texture_default, texture_released, pos_frame);
}

void Scene::addUI_element(std::vector<UI_element*> &new_ui_elements)
{
    loading_logger->debug("Added ui elements[{}] to scene", new_ui_elements.size());

    for (auto g : new_ui_elements)
    {
        Interface->addElement(g);
    }
}

// transfer mouse event to hovered interface part
// if mouse doesn't hover over UI - return false
bool Scene::UI_update_mouse(Vector2f curPos, Event& event, std::string& command_main)
{
    if (event.type == Event::MouseButtonPressed)
    {
        if (Interface->contains(curPos))
        {
            Interface->push_click(curPos);
            return true;
        }
    }
    else if (event.type == Event::MouseButtonReleased)
    {
        if (Interface->is_clicked())
        {
            Interface->release_click(curPos);
            return Interface->contains(curPos);
        }
    }
    return false;
}

void Scene::update(Event& event, std::string& command_main)
{
    /// TEMP
    // transfer mouse clicked event to buttons and sprites
    if (event.type == Event::MouseButtonPressed)
    {
        Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            // if mouse hovers UI (not buttons) then skip
            if (UI_update_mouse(curPos, event, command_main)) return;
            for (auto s : sprites)
            {
                if (s->getGlobalBounds().contains(curPos))
                {
                    s->onClick(true);
                }
            }
            break;

        default:
            break;
        }
    }
    if (event.type == Event::MouseButtonReleased)
    {
        Vector2f curPos = Vector2f(event.mouseButton.x, event.mouseButton.y);
        switch (event.mouseButton.button)
        {
        case Mouse::Left:
            if (UI_update_mouse(curPos, event, command_main)) return;
            for (auto s : sprites)
            {
                // if release mouse over any sprite then switch scene to editor scene
                if (s->getGlobalBounds().contains(curPos))
                {
                    s->onClick(false);
                    command_main = "editor_scene";
                }
            }
            break;

        default:
            break;
        }
    }
}

void Scene::update(Time deltaTime)
{
    for (auto s : sprites)
    {
        s->getGlobalBounds();
        /// ???
        deltaTime += seconds(0.00002f);
        s->update(deltaTime);
    }
}

void Scene::draw_scene_back(RenderTarget& target, RenderStates states) const
{
    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }
}

void Scene::draw_scene_buttons(RenderTarget& target, RenderStates states) const
{
    for (std::size_t i = 0; i < sprites.size(); ++i)
        if (sprites[i])
            target.draw(*sprites[i]);
}

void Scene::draw_scene_Interface(RenderTarget& target, RenderStates states) const
{
    if (Interface)
        Interface->draw(target, states);
}

void Scene::draw(RenderTarget& target, RenderStates states) const
{
    draw_scene_back(target, states);
    draw_scene_buttons(target, states);
    draw_scene_Interface(target, states);
}

/// TEMP
// MyFirstScene constructor
Scene new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions)
{
    Scene main_menu("Main menu");
    main_menu.addTexture(bg, IntRect(0, 0, 1920, 1080));
    main_menu.setScale((float)screen_dimensions.x / 1920, (float)screen_dimensions.y / 1080);

    if (new_button == new_button_pressed)
    {
        spdlog::get("loading")->error("Same textures for pressed/released button");
        throw;
    }

    IntRect button_frame = IntRect(screen_dimensions.x / 2, screen_dimensions.y / 4 * 3, 500, 250);
    main_menu.addButton("button", new_button, new_button_pressed, button_frame);

    return main_menu;
}
