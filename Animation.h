#ifndef ANIMATION_INCLUDE
#define ANIMATION_INCLUDE

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;

class Animation
{
public:
    Animation();

    void addFrame(IntRect rect);
    void setSpriteSheet(Texture& texture);
    Texture* getSpriteSheet();
    std::size_t getSize() const;
    const IntRect& getFrame(std::size_t n) const;

private:
    std::vector<IntRect> m_frames;
    Texture* m_texture;
};

#endif // ANIMATION_INCLUDE
