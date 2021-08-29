#include "Animation.h"

Animation::Animation()
{
    m_frames = std::vector<IntRect>(0);
    textures = std::vector<Texture*>(0);
    texture_index = std::vector<int>(0);
}

void Animation::addFrame(IntRect rect, int i)
{
    m_frames.push_back(rect);
    texture_index.push_back(i);
}

void Animation::setSpriteSheet(Texture& texture, int i)
{
    if (i > textures.size())
        throw;

    if (i == textures.size())
        textures.push_back(&texture);
    else
        textures[i] = &texture;
}

int Animation::addSpriteSheet(Texture& texture)
{
    textures.push_back(&texture);
    return textures.size() - 1;
}

<<<<<<< HEAD
Texture* Animation::getSpriteSheet(int i)
=======
Texture* Animation::getSpriteSheet()
>>>>>>> c167c65bd18d0815f20432e6594df7a741aaccfa
{
    return textures[i];
}

std::size_t Animation::getSize() const
{
    return m_frames.size();
}

const IntRect& Animation::getFrame(std::size_t n) const
{
    return m_frames[n];
}

int Animation::getTextureIndex(std::size_t n) const
{
    return texture_index[n];
}
