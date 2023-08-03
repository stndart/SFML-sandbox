#ifndef CELL_OBJECT_INCLUDE
#define CELL_OBJECT_INCLUDE

#include "AnimatedSprite.h"
#include "extra_algorithms.h"

#include <SFML/Graphics/Sprite.hpp>

class Cell_object : public sf::Drawable, public sf::Transformable
{
private:
    // sprite to display texture
    sf::Sprite sprite;

    sf::Transform parent_cell_transform;

public:
    // object type, eg. "stump"
    std::string type_name;
    // z-coordinate
    int depth_level;

    Cell_object(std::string name, std::shared_ptr<sf::Texture> texture);
    Cell_object(std::string name, std::shared_ptr<sf::Texture> texture, IntRect rect);

    // change object texture and name
    void change_texture(std::string name, std::shared_ptr<sf::Texture> texture);
    // change area on texture to display
    void addTexCoords(sf::IntRect rect);
    // set display size
    void setDisplaySize(sf::Vector2f display_size);

    // used to remember cell coordinates and scale
    void set_parent_transform(sf::Transform parent_transform);

    // overriding Transformable methods
    virtual void move(const sf::Vector2f &offset);
    virtual void rotate(float angle);
    virtual void scale(const sf::Vector2f &factor);
    virtual void setPosition(const sf::Vector2f &position);
    virtual void setPosition(float x, float y);
    virtual void setScale(const sf::Vector2f &factors);
    virtual void setScale(float factorX, float factorY);

    // overriding Drawable methods
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
#endif // CELL_OBJECT_INCLUDE
