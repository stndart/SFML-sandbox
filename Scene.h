#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>

#include "AnimatedSprite.h"
#include "Button.h"
#include "UI_layout.h"

#include <spdlog/spdlog.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

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
    UI_layout* Interface;

    /// TEMP
    // list of buttons
    std::vector <Button*> buttons;
    std::vector <Button*> pushed_buttons;

    // draw different parts interface
    void draw_scene_back(RenderTarget& target, RenderStates states) const;
    void draw_scene_buttons(RenderTarget& target, RenderStates states) const;
    void draw_scene_Interface(RenderTarget& target, RenderStates states) const;

    std::shared_ptr<spdlog::logger> loading_logger, input_logger;

public:
    std::string name;

    Scene(std::string name);

    // adding elements to certain lists
    void addTexture(Texture* texture, IntRect rect);
    void addSprite(AnimatedSprite* sprite);
    void addButton(std::string name, Texture* texture_default, Texture* texture_released, float x, float y);
    void addUI_element(std::vector<UI_element*> &new_ui_elements);
    // transfer mouse event to hovered interface part
    // if mouse doesn't hover over UI - return false
    bool UI_update_mouse(Vector2f curPos, Event& event, std::string& command_main);

    // overriding Drawable methods
    virtual void draw(RenderTarget& target, RenderStates states) const override;
    virtual void update(Event& event, std::string& command_main);
    virtual void update(Time deltaTime);
};

/// TEMP
// MyFirstScene constructor
Scene new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions);

#endif // SCENE_INCLUDE
