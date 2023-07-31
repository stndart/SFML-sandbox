#ifndef CELL_OBJECT_INCLUDE
#define CELL_OBJECT_INCLUDE

#include "AnimatedSprite.h"
#include "extra_algorithms.h"

#include <SFML/Graphics/Sprite.hpp>

class Cell_object : public Drawable, public Transformable
{
private:
    // sprite to display texture
    Sprite sprite;
public:
    // object type, eg. <stump>
    std::string type_name;
    // z-coordinate
    int depth_level;

    Cell_object(std::string name, std::shared_ptr<Texture> texture);
    Cell_object(std::string name, std::shared_ptr<Texture> texture, IntRect rect);

    // change object texture and name
    void change_texture(std::string name, std::shared_ptr<Texture> texture);
    // change m_vertices
    void addTexCoords(IntRect rect);
    // set display size
    void setDisplaySize(Vector2f display_size);

    // overriding Transformable methods
    virtual void move(const Vector2f &offset);
    virtual void rotate(float angle);
    virtual void scale(const Vector2f &factor);
    virtual void setPosition(const Vector2f &position);
    virtual void setPosition(float x, float y);
    virtual void setScale(const Vector2f &factors);
    virtual void setScale(float factorX, float factorY);

    // overriding Drawable methods
    virtual void draw(RenderTarget& target, RenderStates states) const override;
};
#endif // CELL_OBJECT_INCLUDE
