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
    Interface = std::make_shared<UI_window>("Interface", UIFrame, this);

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

    // create and place buttons
    for (nlohmann::json j2 : j["UI elements"])
    {
        shared_ptr<sf::Texture> back = resource_manager->getUITexture(j2["texture"]);

        std::string element_name = j2["type"].get<std::string>() + ":" + j2["texture"].get<std::string>();

        sf::Vector2f texsize(back->getSize());

        sf::Vector2f coords;

        // i didn't know .value<float>(key, 0) exists
        if (j2.contains("Abs x"))
            coords.x += j2["Abs x"].get<float>();
        if (j2.contains("Abs y"))
            coords.y += j2["Abs y"].get<float>();
        if (j2.contains("Rel x"))
            coords.x += j2["Rel x"].get<float>() * screensize.x;
        if (j2.contains("Rel y"))
            coords.y += j2["Rel y"].get<float>() * screensize.y;
        
        sf::Vector2f size;
        if (j2.contains("Abs width"))
            size.x += j2["Abs width"].get<float>();
        if (j2.contains("Abs height"))
            size.y += j2["Abs height"].get<float>();
        if (j2.contains("Rel width"))
            size.x += j2["Rel width"].get<float>() * screensize.x;
        if (j2.contains("Rel height"))
            size.y += j2["Rel height"].get<float>() * screensize.y;

        size = save_aspect_ratio(size, texsize);

        IntRect posrect(coords.x, coords.y, size.x, size.y);
        
        sf::Vector2f origin;
        if (j2.contains("Origin abs x"))
            origin.x += j2["Origin abs x"].get<float>();
        if (j2.contains("Origin abs y"))
            origin.x += j2["Origin abs y"].get<float>();
        if (j2.contains("Origin rel x"))
            origin.x += j2["Origin rel x"].get<float>() * texsize.x;
        if (j2.contains("Origin rel y"))
            origin.x += j2["Origin rel y"].get<float>() * texsize.y;

        // loading_logger->trace(
        //     "Loaded ui element pos {:.0f}x{:.0f}, size {:.0f}x{:.0f}, origin {:.0f}x{:.0f}",
        //     coords.x, coords.y, size.x, size.y, origin.x, origin.y
        // );

        std::shared_ptr<Animation> spritesheet = std::make_shared<Animation>();
        spritesheet->addSpriteSheet(back);
        spritesheet->addFrame(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texsize)), 0);

        if (j2["type"] == "button")
        {
            shared_ptr<sf::Texture> back_pressed;
            if (j2["texture_pressed"].is_string())
                back_pressed = resource_manager->getUITexture(j2["texture_pressed"]);
            else
                back_pressed = back;

            sf::Vector2f texsize_pressed(back_pressed->getSize());

            spritesheet->addSpriteSheet(back_pressed);
            spritesheet->addFrame(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texsize_pressed)), 1);

            std::function<void()> callback;
            std::string callback_name = j2["callback"]["name"];
            if (callback_name == "change scene")
            {
                callback = create_change_scene_callback(*this, j2["callback"]["scene to"].get<std::string>());
            }
            else if (callback_name == "change field")
            {
                Scene_Field* this_field = dynamic_cast<Scene_Field*>(this);
                if (this_field == nullptr)
                {
                    loading_logger->error("Adding illegal callback to button: {}", j2["callback"]["name"]);
                }
                callback = create_change_field_callback(*this_field, j2["callback"]["field to"].get<int>());
            }
            else if (callback_name == "close window")
            {
                callback = create_window_closed_callback(*(scene_controller->get_current_window()));
            }

            std::shared_ptr<UI_button> button = std::make_shared<UI_button>(element_name, posrect, this, spritesheet, callback);
            button->setOrigin(origin);

            Interface->addElement(button);

            loading_logger->trace(
                "Added button {} to scene with coords {:.0f}x{:.0f} and origin {:.0f}x{:.0f}",
                element_name, coords.x, coords.y, origin.x, origin.y
            );
        }
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