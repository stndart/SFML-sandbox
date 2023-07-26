#include "Cell_object.h"

Cell_object::Cell_object(std::string name, Texture* texture, IntRect rect) : type_name(name), depth_level(1)
{
    sprite.setTexture(*texture);
    sprite.setTextureRect(rect);
}

Cell_object::Cell_object(std::string name, Texture* texture) : type_name(name), depth_level(1)
{
    Vector2u texsize = texture->getSize();
    IntRect rect(0, 0, texsize.x, texsize.y);
    sprite.setTexture(*texture);
    sprite.setTextureRect(rect);
}

// change object texture and name
void Cell_object::change_texture(std::string name, Texture* texture)
{
    type_name = name;
    sprite.setTexture(*texture);
}

// change m_vertices
void Cell_object::addTexCoords(IntRect rect)
{
    sprite.setTextureRect(rect);
}

// set display size
void Cell_object::setDisplaySize(Vector2f display_size)
{
    Vector2f new_scale(display_size.x / sprite.getTextureRect().width,
                       display_size.y / sprite.getTextureRect().height);
    setScale(new_scale);
}

// overriding Transformable methods
void Cell_object::move(const Vector2f &offset)
{
    Transformable::move(offset);
    sprite.move(offset);
}

void Cell_object::rotate(float angle)
{
    Transformable::rotate(angle);
    sprite.rotate(angle);
}

void Cell_object::scale(const Vector2f &factor)
{
    Transformable::scale(factor);
    sprite.scale(factor);
}

void Cell_object::setPosition(const Vector2f &position)
{
    Transformable::setPosition(position);
    sprite.setPosition(position);
}

void Cell_object::setPosition(float x, float y)
{
    Transformable::setPosition(x, y);
    sprite.setPosition(x, y);
}

void Cell_object::setScale(const Vector2f &factors)
{
    Transformable::setScale(factors);
    sprite.setScale(factors);
}

void Cell_object::setScale(float factorX, float factorY)
{
    Transformable::setScale(factorX, factorY);
    sprite.setScale(factorX, factorY);
}


void Cell_object::draw(RenderTarget& target, RenderStates states) const
{
    if (sprite.getTexture())
    {
        target.draw(sprite, states);
    }
}
