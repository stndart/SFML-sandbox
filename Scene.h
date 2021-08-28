#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include "AnimatedSprite.h"

using namespace sf;

class Scene : public Drawable, public Transformable
{
private:
    std::vector<Sprite*> sprites;
    const Texture* background;
    Vertex m_vertices[4];
    void draw(RenderTarget& target, RenderStates states) const override;
public:
    std::string name;
    Scene(std::string name);
    void addTexture(Texture* texture, IntRect rect);
    void addSprite(Sprite* sprite);
    virtual void update(Time deltaTime);
};

Scene new_menu_scene(Texture* bg, Texture* new_button, Vector2i screen_dimensions);

#endif // SCENE_INCLUDE
