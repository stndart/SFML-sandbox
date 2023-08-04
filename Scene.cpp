#include "Scene.h"

#include "SceneController.h"
#include "ResourceLoader.h"

int Scene::UI_Z_INDEX = 10;

Scene::Scene(std::string name, Vector2u screensize, std::shared_ptr<ResourceLoader> resload) :
    timer(seconds(0)), resource_manager(resload), screensize(screensize),
    controls_blocked(false), name(name)
{
    // Reaching out to global "loading" logger and "input" logger by names
    loading_logger = spdlog::get("loading");
    input_logger = spdlog::get("input");

    // Create new user interface
    IntRect UIFrame(0, 0, screensize.x, screensize.y);
    Interface = std::make_shared<UI_window>("Interface", UIFrame, this, resload);

    // add to drawables index
    // sorted_drawables.insert(std::make_pair(UI_Z_INDEX, Interface.get()));
}

// returns type name ("Scene" for this class)
std::string Scene::get_type()
{
    return "Scene";
}

// returns config object to be saved externally
nlohmann::json Scene::get_config()
{
    nlohmann::json j;
    return j;
}

// loads interface and other info from config
void Scene::load_config(std::string config_path)
{
    ifstream f(config_path);
    nlohmann::json j = nlohmann::json::parse(f);

    std::string background_texture = j["background"];
    setBackground(resource_manager->getUITexture(background_texture), sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(screensize)));

    Interface->load_config(j);
}

// creates subwindow in Interface by name and loads it's config
// if window already exists, shows it
void Scene::create_subwindow(std::string name, std::string config_path)
{
    std::shared_ptr<UI_window> subwindow = Interface->get_subwindow(name);
    if (subwindow)
    {
        subwindow->show();
        return;
    }

    ifstream f(config_path);
    nlohmann::json j = nlohmann::json::parse(f);
    nlohmann::json j2;
    if (j.contains(name))
        j2 = j.at(name);
    else
    {
        loading_logger->error("Trying to create subwindow of unknown name {}", name);
        return;
    }

    loading_logger->debug("j2 contains UI_elements {}", j2.contains("UI_elements"));

    std::string subwindow_type = j2.value<std::string>("type", "UI window");
    subwindow = subwindow_oftype(name, subwindow_type);
    subwindow->load_config(j2);
    Interface->addElement(subwindow);
}

// returns constructed subwindow of desired type
std::shared_ptr<UI_window> Scene::subwindow_oftype(std::string name, std::string type)
{
    IntRect UIFrame(0, 0, screensize.x, screensize.y);
    if (type == "UI window")
        return std::make_shared<UI_window>(name, UIFrame, this, resource_manager);
    else
    {
        loading_logger->error("Trying to create subwindow of unknown type {}", type);
        return nullptr;
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

// sets default view to match whole screen
void Scene::set_view(sf::View new_view)
{
    default_view = new_view;
}

// change background texture
void Scene::setBackground(std::shared_ptr<Texture> texture, IntRect rect)
{
    background.setTexture(*texture);
    background.setTextureRect(rect);
}

// add sprite to specified framebuffer with specified z-index
void Scene::addSprite(std::shared_ptr<AnimatedSprite> sprite, int z_index, int frame_buffer)
{
    loading_logger->trace("Added sprite \"{}\" with z-index {} to framebuffer {}", sprite->name, z_index, frame_buffer);

    sprites.insert(sprite);
    sprite->z_index = z_index;
}

// add an ui element to <Interface>
void Scene::add_UI_element(std::shared_ptr<UI_element> new_ui_element)
{
    loading_logger->debug("Added ui element {} to scene", new_ui_element->name);

    // save new element in UI tree
    Interface->addElement(new_ui_element);
}

/// TEMP
// clears sprites list
void Scene::delete_sprites()
{
    sprites.clear();
}

// transfer mouse event to hovered interface part
// if mouse doesn't hover over UI - return false
bool Scene::UI_update_mouse(Vector2f curPos, Event& event, std::string& command_main)
{
    if (event.type == Event::MouseButtonPressed)
    {
        if (Interface->contains(curPos))
        {
            Interface->push_click(curPos, controls_blocked);
            return true;
        }
    }
    else if (event.type == Event::MouseButtonReleased)
    {
        if (Interface->is_clicked())
        {
            Interface->release_click(curPos, controls_blocked);
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

// bind callback to keys on the keyboard
void Scene::bind_callback(sf::Keyboard::Key keycode, std::function<void()> callback, Time t)
{
    bound_callbacks[keycode].push_back(std::make_pair(callback, t));
}

// set callbacks array for the key
void Scene::set_bound_callbacks(sf::Keyboard::Key keycode, std::vector<std::pair<std::function<void()>, sf::Time> > callbacks)
{
    reset_bind(keycode);
    bound_callbacks[keycode] = callbacks;
}

// deletes all callbacks bound to key
void Scene::reset_bind(sf::Keyboard::Key keycode)
{
    bound_callbacks[keycode].clear();
    bound_callbacks.erase(keycode);
}

// evaluate all callbacks bount to key
void Scene::evaluate_bound_callbacks(sf::Keyboard::Key keycode)
{
    input_logger->debug("Key {} calling callback", (int)keycode);
    if (bound_callbacks.count(keycode) == 0)
    {
        input_logger->debug("No callbacks bound to key {}", (int)keycode);
        return;
    }

    for (std::pair<std::function<void()>, sf::Time> callpair : bound_callbacks[keycode])
    {
        add_callback(callpair.first, callpair.second);
    }
}

void Scene::update(Event& event, std::string& command_main)
{
    if (event.type == Event::MouseButtonPressed || event.type == Event::MouseButtonReleased)
    {
        sf::Vector2f curpos(event.mouseButton.x, event.mouseButton.y);
        if (Interface->contains(curpos))
        {
            if (event.type == Event::MouseButtonPressed)
                Interface->push_click(curpos, controls_blocked);
        }
        if (Interface->is_clicked() && event.type == Event::MouseButtonReleased)
            Interface->release_click(curpos, controls_blocked);
    }
}

void Scene::update(Time deltaTime)
{
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
    
    Interface->update(deltaTime);
}

// clears and sorts all drawables by z-index
void Scene::sort_drawables()
{
    // now sorted_drawables[%][0] is for default view
    views.push_back(default_view);
    // ask each Drawable source to add their Drawables into map
    // for trash sprites
    for (auto sprite : sprites)
        sorted_drawables[2][0][sprite->z_index].push_back(sprite.get());
    // for interface elements
    Interface->draw_to_zmap(sorted_drawables[UI_Z_INDEX][0]);
}

// draws all framebuffers to textures (because <draw> is const)
void Scene::draw_buffers()
{
    // firstly clear all framebuffers and delete views
    views.clear();
    for (auto& [fb_index, fb] : framebuffers)
    {
        fb.clear(sf::Color::Transparent);
    }
    // then clear sorted_drawbles
    for (auto& [fb_index, z_index_map] : sorted_drawables)
        for (auto& [z_index, draw_list] : z_index_map)
            draw_list.clear();
    // then add each drawable to sorted_drawables
    sort_drawables();
    // now add sorted drawables to appropriate framebuffers
    for (auto& [fb_index, z_index_map] : sorted_drawables)
    {
        // if framebuffer with given index doesn't exist
        if (!framebuffers.contains(fb_index))
        {
            if (!framebuffers[fb_index].create(screensize.x, screensize.y))
            {
                loading_logger->critical("Couldn't construct framebuffer");
                throw;
            }
        }
        for (auto& [view_index, view_list] : z_index_map)
        {
            framebuffers[fb_index].setView(views[view_index]);
            for (auto& [z_index, draw_list] : view_list)
                for (auto sprite : draw_list)
                    framebuffers[fb_index].draw(*sprite);
        }
    }
    // now render all framebuffers
    for (auto& [fb_index, fb] : framebuffers)
    {
        fb.display();
    }
}

void Scene::draw(RenderTarget& target, RenderStates states) const
{
    // draw scene back
    if (background.getTexture())
    {
        target.draw(background, states);
    }

    // cycle through framebuffers and draw each
    for (auto& [fb_index, fb] : framebuffers)
    {
        sf::Sprite fb_sprite(fb.getTexture());
        target.draw(fb_sprite, states);
    }
}