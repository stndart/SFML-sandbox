#include "Animation.h"

Animation::Animation()
{
    m_frames = std::vector<IntRect>(0);
    textures = std::vector<Texture*>(0);
    texture_index = std::vector<int>(0);
    joints = std::vector<Joint>(0);
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

// insert new joint, joints vector is sorted by frames; default frame = -1, meaning end of animation
void Animation::add_joint(int frame, std::string anim_to, int frame_to)
{
    Joint j = {frame, anim_to, frame_to};
    add_joint(j);
}

void Animation::add_joint(Joint j)
{
    if (j.frame == -1)
        j.frame = getSize() - 1;
//    std::cout << "adding joint with f: " << j.frame << std::endl;

    auto j_iter = joints.begin();
    while (j_iter != joints.end() && j_iter->frame < j.frame)
        j_iter++;

//    std::cout << "Animation::add_joint about to insert\n";
    joints.insert(j_iter, j);
}

void Animation::add_joint(std::vector<Joint> js)
{
    for (Joint j : js)
        add_joint(j);
}

// find all joints with certain anim_to ("" = blank filter)
std::vector<Joint> Animation::get_joints(std::string animation) const
{
    std::vector<Joint> res(0);
    for (Joint j : joints)
        if (j.anim_to == animation || animation == "")
            res.push_back(j);
    return res;
}

// find next joint with certain anim_to ("" = blank filter)
Joint Animation::get_next_joint(int cur_frame, std::string animation) const
{
    Joint default_j = {-1, "", 0};
    for (Joint j : joints)
    {
        if (j.frame < cur_frame)
            continue;

        if (animation == j.anim_to || animation == "")
            if (j.frame < default_j.frame || default_j.frame == -1)
                default_j = j;
    }
    return default_j;
}

// find next joint with anim_to in list (blank filter not allowed)
// if multiple joints are found within one frame, they are sorted as <animation>
Joint Animation::get_next_joint(int cur_frame, std::vector<std::string> animation) const
{
//    std::cout << "Animation: We search for next joint with curframe " << cur_frame << " and animations vector";
//    for (std::size_t i = 0; i < animation.size(); ++i)
//        std::cout << ", " << animation[i];
//    std::cout << std::endl;

    Joint default_j = {-1, "", 0};
    std::vector<Joint> res;

    std::size_t anim_i = 0;
    for (Joint j : joints)
    {
        if (j.frame < cur_frame)
            continue;

        anim_i = 0;
        // for any animation that matches j.anim_to update minimum j frame
        for (; anim_i < animation.size(); anim_i++)
            if (animation[anim_i] == j.anim_to)
                if (j.frame <= default_j.frame || default_j.frame == -1)
                {
                    default_j = j;
                    res.push_back(j);
                }
    }
//    if (default_j.frame == -1)
//        std::cout << "Found no joint until the end\n";
//    else
//    {
//        std::cout << "Found joint with frame " << default_j.frame << std::endl;
//        for (Joint j : res)
//            std::cout << ", " << j.anim_to;
//        std::cout << std::endl;
//    }

    for (std::string anim : animation)
        for (Joint j : res)
            if (j.anim_to == anim)
                return j;

    return default_j;
}

// find last joint with certain anim_to ("" = blank filter)
Joint Animation::get_last_joint(int cur_frame, std::string animation) const
{
    Joint default_j = get_next_joint(cur_frame, animation);
    for (Joint j : joints)
        if (j.frame >= cur_frame && (j.anim_to == animation || animation == ""))
            if (j.frame > default_j.frame)
                default_j = j;
    return default_j;
}

// find last joint with anim_to in list (blank filter not allowed)
Joint Animation::get_last_joint(int cur_frame, std::vector<std::string> animation) const
{
    Joint default_j = {-1, "", 0};
    std::size_t anim_i = 0;
    for (Joint j : joints)
    {
        if (j.frame < cur_frame)
            continue;

        anim_i = 0;
        // for any animation that matches j.anim_to update maximum j frame
        for (; anim_i < animation.size(); anim_i++)
            if (animation[anim_i] == j.anim_to && j.frame > default_j.frame)
                default_j = j;
    }
    return default_j;
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
    ///TEMP!!!
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
//        std::cout << "x, y, fx, fy: " << x << "x" << y << ", " << frame_size.x << "x" << frame_size.y << std::endl;
        addFrame(IntRect(x, y, frame_size.x, frame_size.y), textures.size() - 1);
    }
}
