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
    // list of sprites
    std::vector<AnimatedSprite*> sprites;
    // list of sprites that belong to framebuffer
    std::vector<AnimatedSprite*> sprites_to_framebuffer;
    // framebuffer, to which all <sprites> are rendered (for example, with no-blend mode)
    sf::RenderTexture framebuffer;

    // set of all animated objects sorted by z-index.
    // Z-index of Field is typically 0, z-index of UI is typically 10
    std::set <std::pair<int, Drawable*> > sorted_drawables;
    // set of all drawables sorted by z-index to draw in framebuffer
    std::set <std::pair<int, Drawable*> > sorted_drawables_to_framebuffer;

    Texture* background;
    Vertex m_vertices[4];
    // Object of user interface. Drawable
    UI_window* Interface;

    SceneController* scene_controller;

    std::shared_ptr<spdlog::logger> loading_logger, input_logger;

public:
    std::string name;

    Scene(std::string name, Vector2i screensize);

    // sets scene controller to invoke callbacks of switching scenes
    void set_scene_controller(SceneController& sc);
    SceneController& get_scene_controller() const;

    // adding elements to certain lists
    void addTexture(Texture* texture, IntRect rect);
    void addSprite(AnimatedSprite* sprite, int z_index=1, bool to_frame_buffer=false);
    /// TEMP
    // deletes all sprites either from framebuffer or not
    void delete_sprites(bool from_frame_buffer=false);

    // add button with callback and return its pointer
    std::shared_ptr<UI_button> addButton(std::string name, Texture* texture_default, Texture* texture_released, IntRect pos_frame,
                                         std::string origin = "center", std::function<void()> callback = std::function<void()>{nullptr});
    std::shared_ptr<UI_button> addButton(std::string name, Texture* texture_default, Texture* texture_released, int pos_x, int pos_y,
                                         std::string origin = "center", std::function<void()> callback = std::function<void()>{nullptr});

    void addUI_element(std::vector<std::shared_ptr<UI_element> > &new_ui_elements);
    // transfer mouse event to hovered interface part
    // if mouse doesn't hover over UI - return false
    bool UI_update_mouse(Vector2f curPos, Event& event, std::string& command_main);

    // schedule callback to call after <t> seconds
    void add_callback(std::function<void()> callback, Time t = seconds(0.5));
    // cancels callback
    void cancel_callbacks();
    // checks if there are callbacks scheduled. For external purposes
    bool has_callbacks() const;

    // draw all framebuffers (because <draw> is const)
    virtual void draw_buffers();
    // overriding Drawable methods
    virtual void update(Event& event, std::string& command_main);
    virtual void update(Time deltaTime);
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};

/// TEMP
// MyFirstScene constructor
std::shared_ptr<Scene> new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions);

#endif // SCENE_INCLUDE
