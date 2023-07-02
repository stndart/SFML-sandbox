#include "Animation.h"

Animation::Animation()
{
    m_frames = std::vector<IntRect>(0);
    textures = std::vector<Texture*>(0);
    texture_index = std::vector<int>(0);
}

// set spritesheet #i with texture by link
void Animation::setSpriteSheet(Texture& texture, std::size_t i)
{
    if (i > textures.size())
        throw;

    if (i == textures.size())
        textures.push_back(&texture);
    else
        textures[i] = &texture;
}

// append spritesheet with texture by link
int Animation::addSpriteSheet(Texture& texture)
{
    textures.push_back(&texture);
    return textures.size() - 1;
}

// get spritesheet by index
Texture* Animation::getSpriteSheet(int i)
{
    return textures[texture_index[i]];
}

// get number of frames
std::size_t Animation::getSize() const
{
    return m_frames.size();
}

// adds new spritesheet coordinates as new frame from spritesheet #i
void Animation::addFrame(IntRect rect, int i)
{
    m_frames.push_back(rect);
    texture_index.push_back(i);
}

// return frame coordinates of n-th frame
const IntRect& Animation::getFrame(std::size_t n) const
{
    return m_frames[n];
}

// return spritesheet index of n frame
int Animation::getTextureIndex(std::size_t n) const
{
    return texture_index[n];
}

/// TEMP (as no resource manager implemented)
// loads spritesheets and frames from list by filenames [idle_animation, movement_0]
// frames are loaded from spritesheet automatically, row by row
void Animation::load_from_file(std::string animation_filename, Vector2u frame_size)
{
    // get animation_filename, frame_size, N from json
    ///TEMP
    int N;
    // if <animation_filename> starts with ".../idle_animation"
    if (animation_filename.rfind("Images/Flametail/idle_animation", 0) == 0)
        N = 36;
    else
        N = 40;
    std::string fname = animation_filename;

    Texture* ssheet = new Texture;
    if (!ssheet->loadFromFile(fname))
    {
        std::cout << "Failed to load animation texture\n";
        throw;
    }
    Vector2u sheet_size = ssheet->getSize();
    int Nx = sheet_size.x / frame_size.x;
    int Ny = sheet_size.y / frame_size.y;
    /// TEMP
    if (Nx == 1)
        N = 1;

    if (Nx * Ny < N)
    {
        std::cout << "Spritesheet contains less frames than requested\n";
        std::cout << "In spritesheet row: " << Nx << ", requested: " << N << std::endl;
        std::cout << "Sritesheet size " << sheet_size.x << "x" << sheet_size.y << std::endl;
        std::cout << "Frame size " << frame_size.x << "x" << frame_size.y << std::endl;
        throw;
    }

    addSpriteSheet(*ssheet);
    for (int i = 0; i < N; ++i)
    {
        int x = frame_size.x * (i % Nx);
        int y = frame_size.y * (int)(i / Nx);
        std::cout << "x, y, fx, fy: " << x << "x" << y << ", " << frame_size.x << "x" << frame_size.y << std::endl;
        addFrame(IntRect(x, y, frame_size.x, frame_size.y), textures.size() - 1);
    }
}
