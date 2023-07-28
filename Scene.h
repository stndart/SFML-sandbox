#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>
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
private:
    // list of all animated objects
    std::vector <AnimatedSprite*> sprites;

protected:
    Texture* background;
    Vertex m_vertices[4];
    // Object of user interface. Drawable
    UI_window* Interface;

    // draw different parts interface
    void draw_scene_back(RenderTarget& target, RenderStates states) const;
    void draw_scene_buttons(RenderTarget& target, RenderStates states) const;
    void draw_scene_Interface(RenderTarget& target, RenderStates states) const;

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
    void addSprite(AnimatedSprite* sprite);
    void addButton(std::string name, Texture* texture_default, Texture* texture_released, IntRect pos_frame, std::function<void()> callback = std::function<void()>{nullptr}, std::string origin = "center");
    void addButton(std::string name, Texture* texture_default, Texture* texture_released, int pos_x, int pos_y, std::function<void()> callback = std::function<void()>{nullptr}, std::string origin = "center");
    void addUI_element(std::vector<UI_element*> &new_ui_elements);
    // transfer mouse event to hovered interface part
    // if mouse doesn't hover over UI - return false
    bool UI_update_mouse(Vector2f curPos, Event& event, std::string& command_main);

    // overriding Drawable methods
    void draw(RenderTarget& target, RenderStates states) const override;
    virtual void update(Event& event, std::string& command_main);
    virtual void update(Time deltaTime);
};

/// TEMP
// MyFirstScene constructor
std::shared_ptr<Scene> new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2u screen_dimensions);

#endif // SCENE_INCLUDE
