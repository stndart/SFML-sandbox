#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>
#include <deque>
#include <map>
#include <functional>
#include <fstream>

#include "AnimatedSprite.h"
#include "UI_window.h"
#include "extra_algorithms.h"
#include "Callbacks.h"
#include "Animation.h"

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

class SceneController;
class ResourceLoader;

using namespace sf;

class Scene : public Drawable, public Transformable
{
    static int UI_Z_INDEX;

private:
    // map of callbacks by timers. When timer expires, callback is evaluated and poped from map
    std::map <Time, std::vector<std::function<void()> > > scheduled_callbacks;
    // queue of callbacks to call
    // they need to be stored before evaluation since exist callbacks, that invalidate others
    std::deque<std::function<void()> > callbacks_to_call;
    // shift timer used to sweep through map of callbacks
    Time timer;

protected:
    // pointer to resource manager
    std::shared_ptr<ResourceLoader> resource_manager;

    // screen size in pixels. Passed in constructor
    sf::Vector2u screensize;

    // list of sprites (effects, usually)
    std::set<std::shared_ptr<AnimatedSprite> > sprites;
    // framebuffers, used to split drawing different parts of game. Mapped to their render order
    std::map<int, sf::RenderTexture> framebuffers;
    // list of views, used to draw field and field objects (cells, etc.) independently
    std::vector<sf::View> views;
    // default view that is set for screen effects
    sf::View default_view;

    // All animated objects sorted by z-index.
    // first index is number of a framebuffer, second is view index, third index is z-index, fourth iterable is set of weak pointers
    // Scene is typically drawn on [1]th framebuffer, while UI is on [3]rd
    // sorted_drawables are cleared and refilled each frame
    std::map<int, std::map<int, std::map<int, std::vector<const Drawable*> > > > sorted_drawables;

    // draws background
    sf::Sprite background;
    // Object of user interface. Drawable
    std::shared_ptr<UI_window> Interface;

    // returns constructed subwindow of desired type
    virtual std::shared_ptr<UI_window> subwindow_oftype(std::string name, std::string type);

    SceneController* scene_controller;

    // flag if WASD blocked
    bool controls_blocked;

    // list of key bindings
    std::map<sf::Keyboard::Key, std::vector<std::pair<std::function<void()>, sf::Time> > > bound_callbacks;

    std::shared_ptr<spdlog::logger> loading_logger, input_logger;

public:
    std::string name;

    Scene(std::string name, Vector2u screensize, std::shared_ptr<ResourceLoader> resload);

    // returns type name ("Scene" for this class)
    virtual std::string get_type();

    // returns config object to be saved externally
    virtual nlohmann::json get_config();
    // loads interface and other info from config
    virtual void load_config(std::string config_path);
    // creates subwindow in Interface by name and loads it's config
    // if window already exists, shows it
    std::shared_ptr<UI_window> create_subwindow(std::string name, std::string config_path = "configs/dynamic_UI.json", std::string config_name="");
    // shows or hides UI_window by name
    void show_UI_window(std::string name, bool show=true);

    // sets scene controller to invoke callbacks of switching scenes
    void set_scene_controller(SceneController& sc);
    SceneController& get_scene_controller() const;

    // sets default view to match whole screen
    void set_view(sf::View new_view);

    // change background texture
    void setBackground(std::shared_ptr<Texture> texture, IntRect rect);
    // add sprite to specified framebuffer with specified z-index
    void addSprite(std::shared_ptr<AnimatedSprite> sprite, int z_index=1, int framebuffer = 2);
    // add an ui element to <Interface>
    void add_UI_element(std::shared_ptr<UI_element> new_ui_element);
    /// TEMP
    // clear sprites list
    void delete_sprites();

    // transfer mouse event to hovered interface part
    // if mouse doesn't hover over UI - return false
    bool UI_update_mouse(Vector2f curPos, Event& event);

    // schedule callback to call after <t> seconds
    void add_callback(std::function<void()> callback, Time t = seconds(0.5));
    // cancels callback
    void cancel_callbacks();
    // checks if there are callbacks scheduled. For external purposes
    bool has_callbacks() const;

    // bind callback to keys on the keyboard
    void bind_callback(sf::Keyboard::Key keycode, std::function<void()> callback, Time t = seconds(0.5));
    // set callbacks array for the key
    void set_bound_callbacks(sf::Keyboard::Key keycode, std::function<void()> callback);
    void set_bound_callbacks(sf::Keyboard::Key keycode, std::pair<std::function<void()>, sf::Time> callback);
    void set_bound_callbacks(sf::Keyboard::Key keycode, std::vector<std::pair<std::function<void()>, sf::Time> > callbacks);
    // deletes all callbacks bound to key
    void reset_bind(sf::Keyboard::Key keycode);
    // evaluate all callbacks bount to key
    void evaluate_bound_callbacks(sf::Keyboard::Key keycode);

    // clears and sorts all drawables by z-index
    virtual void sort_drawables();
    // draws all framebuffers to textures (because <draw> is const)
    virtual void draw_buffers();
    // overriding Drawable methods
    virtual void update(Event& event);
    virtual void update(Time deltaTime);
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // SCENE_INCLUDE
