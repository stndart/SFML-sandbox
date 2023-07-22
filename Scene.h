#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>
#include <deque>
#include <map>
#include <functional>

#include "AnimatedSprite.h"
#include "UI_window.h"
#include "extra_algorithms.h"

#include <spdlog/spdlog.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

class SceneController;

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
    // set of all animated objects sorted by z-index.
    // Z-index of Field is typically 0, z-index of UI is typically 10
    std::set <std::pair<int, Drawable*> > sorted_drawables;
    // list of all sprites
    std::vector <AnimatedSprite*> sprites;

    Texture* background;
    Vertex m_vertices[4];
    // Object of user interface. Drawable
    UI_window* Interface;

    SceneController* scene_controller;

    std::shared_ptr<spdlog::logger> loading_logger, input_logger;

public:
    std::string name;

    Scene(std::string name);

    // sets scene controller to invoke callbacks of switching scenes
    void set_scene_controller(SceneController& sc);
    SceneController& get_scene_controller() const;

    // adding elements to certain lists
    void addTexture(Texture* texture, IntRect rect);
    void addSprite(AnimatedSprite* sprite, int z_index=1);
    /// TEMP
    void delete_sprites();

    void addButton(std::string name, Texture* texture_default, Texture* texture_released, IntRect pos_frame, std::function<void()> callback = std::function<void()>{nullptr}, std::string origin = "center");
    void addButton(std::string name, Texture* texture_default, Texture* texture_released, int pos_x, int pos_y, std::function<void()> callback = std::function<void()>{nullptr}, std::string origin = "center");
    void addUI_element(std::vector<UI_element*> &new_ui_elements);
    // transfer mouse event to hovered interface part
    // if mouse doesn't hover over UI - return false
    bool UI_update_mouse(Vector2f curPos, Event& event, std::string& command_main);

    // schedule callback to call after <t> seconds
    void add_callback(std::function<void()> callback, Time t = seconds(0.5));
    // cancels callback
    void cancel_callbacks();

    // overriding Drawable methods
    virtual void update(Event& event, std::string& command_main);
    virtual void update(Time deltaTime);
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

/// TEMP
// MyFirstScene constructor
std::shared_ptr<Scene> new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions);

#endif // SCENE_INCLUDE
