#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "AnimatedSprite.h"
#include "Button.h"

using namespace sf;

class Scene : public Drawable, public Transformable
{
private:
    std::vector <AnimatedSprite*> sprites;
protected:
    Texture* background;
    Vertex m_vertices[4];

    std::vector <Button*> buttons;
    std::vector <Button*> pushed_buttons;

    void draw_scene_back(RenderTarget& target, RenderStates states) const;
    void draw_scene_buttons(RenderTarget& target, RenderStates states) const;
public:
    std::string name;
    Scene(std::string name);
    void addTexture(Texture* texture, IntRect rect);
    void addSprite(AnimatedSprite* sprite);
    void addButton(std::string name, Texture* texture_default, Texture* texture_released, float x, float y);

    virtual void draw(RenderTarget& target, RenderStates states) const override;
    virtual void update(Event& event, std::string& command_main);
    virtual void update(Time deltaTime);
};

Scene new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions);

#endif // SCENE_INCLUDE
