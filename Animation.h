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
    void setSpriteSheet(Texture& texture, int i=0);
    int addSpriteSheet(Texture& texture);
    Texture* getSpriteSheet(int i=0);
    std::size_t getSize() const;
    const IntRect& getFrame(std::size_t n) const;
    int getTextureIndex(std::size_t n) const;

private:
<<<<<<< HEAD
    std::vector<IntRect> m_frames;
    std::vector<Texture*> textures;
    std::vector<int> texture_index;
=======
    std::vector<IntRect> m_frames;
    Texture* m_texture;
>>>>>>> c167c65bd18d0815f20432e6594df7a741aaccfa
};

#endif // ANIMATION_INCLUDE
