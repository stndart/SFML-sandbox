#ifndef ANIMATION_INCLUDE
#define ANIMATION_INCLUDE

#include <iostream>
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;

// Animation is a structure with a list of spritesheets inside and an array of frames coordinates on appropriate spritesheet
class Animation
{
private:
    // frames coordinates on spritesheet list
    std::vector<IntRect> m_frames;
    // spritesheet list
    std::vector<Texture*> textures;
    // frames spritesheet index list
    std::vector<int> texture_index;

public:
    Animation();

    // set texture by link for spritesheet #i
    void setSpriteSheet(Texture& texture, std::size_t i=0);
    // append spritesheet with texture by link
    int addSpriteSheet(Texture& texture);
    // get spritesheet by index
    Texture* getSpriteSheet(int i=0);
    // get number of frames
    std::size_t getSize() const;

    // adds new spritesheet coordinates as new frame from spritesheet #i
    void addFrame(IntRect rect, int i=0);
    // return frame coordinates of n-th frame
    const IntRect& getFrame(std::size_t n) const;
    // return spritesheet index of n frame
    int getTextureIndex(std::size_t n) const;

    /// TEMP (as no resource manager implemented)
    // loads spritesheets and frames from filename
    // frames are loaded from spritesheet automatically, row by row
    void load_from_file(std::string animation_filename, Vector2u framesize);
};

#endif // ANIMATION_INCLUDE
