#include "Scene.h"

Scene::Scene(std::string name) : background(NULL), name(name)
{
    loading_logger = spdlog::get("loading");


    UI_layout* new_interface = new UI_layout();
    Interface = new_interface;
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
    sprites.push_back(sprite);
}

// add and place button
void Scene::addButton(std::string name, Texture* texture_default, Texture* texture_released, float x, float y)
{
    Button* new_button = new Button(name, texture_default, texture_released);
    new_button->change_position(sf::Vector2f{x, y});
    buttons.push_back(new_button);
}

void Scene::addUI_element(std::vector<UI_element*> &new_ui_elements)
{
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
        if (Interface->get_isClicked())
        {
            std::string ans = Interface->release_click();
            if (Interface->contains(curPos)) command_main = ans;
            return true;
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
            for (auto b : buttons)
            {
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
            while (pushed_buttons.size() > 0)
            {
                // if cursor still hovers pushed button then extract action from button
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

    for (std::size_t i = 0; i < buttons.size(); ++i)
        if (buttons[i])
            target.draw(*buttons[i]);
}

void Scene::draw_scene_Interface(RenderTarget& target, RenderStates states) const
{
    if (Interface)
        target.draw(*Interface);
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

    AnimatedSprite* button = new AnimatedSprite("asprite", *new_button, IntRect(0, 0, 1000, 500));
    button->getAnimation()->addSpriteSheet(*new_button_pressed);
    button->getAnimation()->addFrame(IntRect(0, 0, 1000, 500), 1);
    button->setLooped(false);
    button->setScale(0.5f, 0.5f);
    button->setOrigin(button->getLocalBounds().width / 2.0f,
                      button->getLocalBounds().height / 2.0f);
    button->setPosition(Vector2f(screen_dimensions.x / 2, screen_dimensions.y / 4 * 3));
    button->setFrameTime(milliseconds(1.0f));

    main_menu.addSprite(button);

    return main_menu;
}
