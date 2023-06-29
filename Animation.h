#ifndef ANIMATION_INCLUDE
#define ANIMATION_INCLUDE

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;

// Animation is a structure with a list of spritesheets inside and an array of frames coordinates on appropriate spritesheet
class Animation
{
public:
    Animation();

    // adds new spritesheet coordinates as new frame from spritesheet #i
    void addFrame(IntRect rect, int i=0);
    // set texture by link for spritesheet #i
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
