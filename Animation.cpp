#include "Animation.h"

#include "ResourceLoader.h"

Animation::Animation()
{
    // Reaching out to global "loading" logger and "graphics" logger by names
    loading_logger = spdlog::get("loading");
    graphics_logger = spdlog::get("graphics");
}

Animation::Animation(std::shared_ptr<Texture> texture) : Animation()
{
    setSpriteSheet(texture, 0);
}

// set spritesheet #i with texture by link
void Animation::setSpriteSheet(std::shared_ptr<Texture> texture, std::size_t i)
{
    if (i > textures.size())
        throw;

    if (i == textures.size())
        textures.push_back(texture);
    else
        textures[i] = texture;
}

// append spritesheet with texture by link
int Animation::addSpriteSheet(std::shared_ptr<Texture> texture)
{
    textures.push_back(texture);
    return textures.size() - 1;
}

// get spritesheet by index
std::shared_ptr<Texture> Animation::getSpriteSheet(int i)
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

    auto j_iter = joints.begin();
    while (j_iter != joints.end() && j_iter->frame < j.frame)
        j_iter++;

    //graphics_logger->trace("Animation::add_joint about to insert");

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
    graphics_logger->trace("Animation: We search for next joint with curframe {} and animations vector", cur_frame);

    std::string s = "";
    for (std::size_t i = 0; i < animation.size(); ++i)
        s += ", " + animation[i];

    graphics_logger->trace(s);

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
    if (default_j.frame == -1)
        graphics_logger->trace("Found no joint until the end");
    else
    {
        graphics_logger->trace("Found joint with frame {}", default_j.frame);

        s = "";
        for (Joint j : res)
            s +=  ", " + j.anim_to;
        graphics_logger->trace(s);
    }

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
    if (n >= m_frames.size())
    {
        loading_logger->error("ERROR: Animation::getFrame with n = {} and m_frames size = {}", n, m_frames.size());
        throw;
    }
    return m_frames[n];
}

// return spritesheet of n-th frame
std::shared_ptr<Texture> Animation::getTexture(std::size_t n) const
{
    return textures[texture_index[n]];
}

// return spritesheet index of n frame
int Animation::getTextureIndex(std::size_t n) const
{
    return texture_index[n];
}

// loads animation from spritesheet with specified frame size and number
void Animation::load_from_texture(std::shared_ptr<ResourceLoader> resload, std::string name, Vector2i frame_size, int frame_count)
{
    std::shared_ptr<Texture> sheet = resload->getCharacterTexture(name);

    Vector2u sheet_size = sheet->getSize();
    int Nx = sheet_size.x / frame_size.x;
    int Ny = sheet_size.y / frame_size.y;

    if (Nx * Ny < frame_count)
    {
        loading_logger->error("Spritesheet contains less frames than requested\n");
        loading_logger->error("In spritesheet row: {}, requested: {}", Nx, frame_count);
        loading_logger->error("Sritesheet size {}x{}", sheet_size.x, sheet_size.y);
        loading_logger->error("Frame size {}x{}", frame_size.x, frame_size.y);
        return;
    }

    addSpriteSheet(sheet);
    for (int i = 0; i < frame_count; ++i)
    {
        int x = frame_size.x * (i % Nx);
        int y = frame_size.y * (int)(i / Nx);
        addFrame(IntRect(x, y, frame_size.x, frame_size.y), textures.size() - 1);
    }
}
