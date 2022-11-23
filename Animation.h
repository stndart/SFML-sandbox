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

    void addFrame(IntRect rect, int i=0);
    void setSpriteSheet(Texture& texture, std::size_t i=0);
    int addSpriteSheet(Texture& texture);
    Texture* getSpriteSheet(int i=0);
    std::size_t getSize() const;
    const IntRect& getFrame(std::size_t n) const;
    int getTextureIndex(std::size_t n) const;

private:
    // frames coordinates on spritesheet list
    std::vector<IntRect> m_frames;
    // spritesheet list
    std::vector<Texture*> textures;
    // frames spritesheet index list
    std::vector<int> texture_index;
};

#endif // ANIMATION_INCLUDE
