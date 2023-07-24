#include "Scene.h"
#include "SceneController.h"

int Scene::UI_Z_INDEX = 10;

Scene::Scene(std::string name, Vector2i screensize) : timer(seconds(0)), background(NULL), name(name)
{
    // Reaching out to global "loading" logger and "input" logger by names
    loading_logger = spdlog::get("loading");
    input_logger = spdlog::get("input");

    // Create new user interface
    IntRect UIFrame(0, 0, screensize.x, screensize.y);
    Interface = new UI_window("Interface", UIFrame, this);

    // add to drawables index
    sorted_drawables.insert(std::make_pair(UI_Z_INDEX, Interface));

    if (!framebuffer.create(screensize.x, screensize.y))
    {
        loading_logger->critical("Couldn't construct framebuffer");
        throw;
    }
}

// sets scene controller to invoke callbacks of switching scenes
void Scene::set_scene_controller(SceneController& sc)
{
    scene_controller = &sc;
}

SceneController& Scene::get_scene_controller() const
{
    if (!scene_controller)
    {
        loading_logger->error("Scene:: trying to get empty scene_controller");
        throw;
    }

    return *scene_controller;
}

// change texture and update background from it
void Scene::addTexture(Texture* texture, IntRect rect)
{
    background = texture;

    cutout_texture_to_frame(m_vertices, rect);
}

void Scene::addSprite(AnimatedSprite* sprite, int z_index, bool to_frame_buffer)
{
    loading_logger->debug("Added sprite \"{}\" with z-index {} to framebuffer {}", sprite->name, z_index, (int)to_frame_buffer);

    if (!to_frame_buffer)
    {
        sprites.push_back(sprite);
        sprite->z_index = z_index;
        sorted_drawables.insert(make_pair(z_index, sprite));
    }
    else
    {
        sprites_to_framebuffer.push_back(sprite);
        sprite->z_index = z_index;
        sorted_drawables_to_framebuffer.insert(make_pair(z_index, sprite));
    }
}

// deletes all sprites either from framebuffer or not
void Scene::delete_sprites(bool from_frame_buffer)
{
    if (!from_frame_buffer)
    {
        for (auto s : sprites)
        {
            sorted_drawables.erase(make_pair(s->z_index, s));
        }
        sprites.clear();
    }
    else
    {
        for (auto s : sprites_to_framebuffer)
        {
            sorted_drawables_to_framebuffer.erase(make_pair(s->z_index, s));
        }
        sprites_to_framebuffer.clear();
    }
}

// add and place button
std::shared_ptr<UI_button> Scene::addButton(std::string name, Texture* texture_default, Texture* texture_released, IntRect pos_frame,
                                            std::string origin, std::function<void()> ncallback)
{
    loading_logger->debug("Added button \"{}\" to scene", name);

    // get animation frame from texture size
    Vector2u tex_size = texture_default->getSize();
    IntRect tex_frame(0, 0, tex_size.x, tex_size.y);

    /// MEMORY LEAK
    Animation* tAn = new Animation;
    tAn->addSpriteSheet(texture_default);
    tAn->addFrame(tex_frame, 0);
    tAn->addSpriteSheet(texture_released);
    tAn->addFrame(tex_frame, 1);

    loading_logger->warn("Created new Animation for button: MEMORY LEAK");

    std::shared_ptr<UI_button> new_button = std::make_shared<UI_button>(UI_button(name, pos_frame, this, tAn, ncallback));
    // default origin is "center"
    if (origin == "center")
    {
        loading_logger->trace("trying center");
        new_button->setOrigin((float)pos_frame.width / 2.f, (float)pos_frame.height / 2.f);
    }
    else if (origin == "top left")
    {
        loading_logger->trace("trying top left");
        new_button->setOrigin(0, 0);
    }
    else
    {
        loading_logger->warn("Unknown button alignment origin {}, set to \"center\"", origin);
    }

    // register new button to Interface
    Interface->addElement(new_button);

    return new_button;
}

std::shared_ptr<UI_button> Scene::addButton(std::string name, Texture* texture_default, Texture* texture_released, int pos_x, int pos_y,
                                            std::string origin, std::function<void()> ncallback)
{
    // creating animation frame from args
    Vector2u tex_size = texture_default->getSize();
    IntRect pos_frame(pos_x, pos_y, tex_size.x, tex_size.y);

    return addButton(name, texture_default, texture_released, pos_frame, origin, ncallback);
}

void Scene::addUI_element(std::vector<std::shared_ptr<UI_element> > &new_ui_elements)
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

// schedule callback to call after <t> seconds
void Scene::add_callback(std::function<void()> callback, Time t)
{
    if (!callback)
    {
        input_logger->error("Adding empty callback");
        throw;
    }

    scheduled_callbacks[timer + t].push_back(std::move(callback));
}

// cancels callback
void Scene::cancel_callbacks()
{
    scheduled_callbacks.clear();
    callbacks_to_call.clear();
}

bool Scene::has_callbacks() const
{
    return !callbacks_to_call.empty();
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
            break;

        default:
            break;
        }
    }
}

void Scene::update(Time deltaTime)
{
//    input_logger->trace("Scene::update callbacks size {}", scheduled_callbacks.size());

    // scheduled callbacks logic
    timer += deltaTime;

    callbacks_to_call.clear();
    // now check for expired timers and remove after callback calls
    while (!scheduled_callbacks.empty() && scheduled_callbacks.begin()->first <= timer)
    {
        for (auto callback : scheduled_callbacks.begin()->second)
            callbacks_to_call.push_back(std::move(callback));

        // erase outdated timers
        scheduled_callbacks.erase(scheduled_callbacks.begin());
    }
    // now evaluating saved callbacks
    while (!callbacks_to_call.empty())
    {
        callbacks_to_call.front()();

        // because callback can empty this deque
        if (!callbacks_to_call.empty())
            callbacks_to_call.pop_front();
    }

    for (auto s : sprites)
    {
        s->update(deltaTime);
    }
    for (auto s : sprites_to_framebuffer)
    {
        s->update(deltaTime);
    }
}

// draw all framebuffers (because <draw> is const)
void Scene::draw_buffers()
{
    RenderStates states;
    framebuffer.clear(sf::Color::Transparent);

    auto p = sorted_drawables_to_framebuffer.begin();
    for (; p != sorted_drawables_to_framebuffer.end(); ++p)
    {
        if (p->first > UI_Z_INDEX)
            break;

        // if valid drawable, then draw it
        if (p->second)
            framebuffer.draw(*p->second);
    }

    framebuffer.display();
}

void Scene::draw(RenderTarget& target, RenderStates states) const
{
    // draw scene back
    if (background)
    {
        states.transform *= getTransform();
        states.texture = background;
        target.draw(m_vertices, 4, Quads, states);
    }

    auto p = sorted_drawables.begin();
    for (; p != sorted_drawables.end(); ++p)
    {
        //break; /// dont draw field
        if (p->first > UI_Z_INDEX)
            break;

        // if valid drawable, then draw it
        if (p->second)
            target.draw(*p->second);
    }

    sf::Sprite framebuffer_sprite(framebuffer.getTexture());
    target.draw(framebuffer_sprite, states);
}

/// TEMP
// MyFirstScene constructor
std::shared_ptr<Scene> new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions)
{
    std::shared_ptr<Scene> main_menu = std::make_shared<Scene>("Main menu", screen_dimensions);
    main_menu->addTexture(bg, IntRect(0, 0, 1920, 1080));
    main_menu->setScale((float)screen_dimensions.x / 1920, (float)screen_dimensions.y / 1080);

    /// is it really an error?
    if (new_button == new_button_pressed)
    {
        spdlog::get("loading")->error("Same textures for pressed/released button");
        throw;
    }

    spdlog::get("loading")->debug("Making NEWGAME button with callback");

    IntRect button_frame = IntRect(screen_dimensions.x / 2, screen_dimensions.y / 4 * 3, 500, 250);

    std::function<void()> ncallback = create_change_scene_callback(main_menu, "Scene_editor");
    spdlog::get("loading")->debug("Constructed callback");

    main_menu->addButton("button", new_button, new_button_pressed, button_frame, "center", ncallback);
    spdlog::get("loading")->debug("Constructed button");

    return main_menu;
}
