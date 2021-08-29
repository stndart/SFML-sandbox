#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>
#include "AnimatedSprite.h"

using namespace sf;

class Scene : public Drawable, public Transformable
{
private:
    std::vector<AnimatedSprite*> sprites;
    const Texture* background;
    Vertex m_vertices[4];
    void draw(RenderTarget& target, RenderStates states) const override;
public:
    std::string name;
    Scene(std::string name);
    void addTexture(Texture* texture, IntRect rect);
<<<<<<< HEAD
    void addSprite(AnimatedSprite* sprite);
    virtual void update(Event& event);
    virtual void update(Time deltaTime);
=======
    void addSprite(Sprite* sprite);
    virtual void update(Event& event);
>>>>>>> c167c65bd18d0815f20432e6594df7a741aaccfa
};

Scene new_menu_scene(Texture* bg, Texture* new_button, Texture* new_button_pressed, Vector2i screen_dimensions);

#endif // SCENE_INCLUDE
