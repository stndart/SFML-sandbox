#ifndef ANIMATION_INCLUDE
#define ANIMATION_INCLUDE

#include <iostream>
#include <vector>

#include <spdlog/spdlog.h>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;

class ResourceLoader;

struct Joint
{
    int frame;
    std::string anim_to;
    int frame_to;
};

// Animation is used to store mutliple textures or frames
// Different frames can use same texture (spritesheet)
// AnimatedSprite uses Animation to store sequences of frames and switch through them
// Animation also contains array of Joints: key frames, that are bound to other Animations (by name)
// use addSpriteSheet(texture) to add texture to Animation
// use addFrame(frame, tex_i) to add frame with tex_i th spritesheet

class Animation
{
private:
    // frames coordinates on spritesheet list
    std::vector<IntRect> m_frames;
    // spritesheet list
    std::vector<std::shared_ptr<Texture> > textures;
    // frames spritesheet index list
    std::vector<int> texture_index;

    // joints, i.e. frames with available transition to other animation
    std::vector<Joint> joints;

    std::shared_ptr<spdlog::logger> loading_logger, graphics_logger;

public:
    Animation();
    Animation(std::shared_ptr<Texture> texture, bool addframe = false);

    // set texture by link for spritesheet #i
    void setSpriteSheet(std::shared_ptr<Texture> texture, std::size_t i=0);
    // append spritesheet with texture by link
    int addSpriteSheet(std::shared_ptr<Texture> texture);
    // get spritesheet by index
    std::shared_ptr<Texture> getSpriteSheet(int i=0);
    // get number of frames
    std::size_t getSize() const;

    // insert new joint, joints vector is sorted by frames
    void add_joint(Joint j);
    void add_joint(int frame, std::string anim_to, int frame_to);
    void add_joint(std::vector<Joint> js);
    // find all joints with certain anim_to ("" = blank filter)
    std::vector<Joint> get_joints(std::string animation="") const;

    // find next joint with certain anim_to ("" = blank filter)
    Joint get_next_joint(int cur_frame, std::string animation="") const;
    // find next joint with anim_to in list (blank filter not allowed)
    Joint get_next_joint(int cur_frame, std::vector<std::string> animation) const;

    // find last joint with certain anim_to ("" = blank filter)
    Joint get_last_joint(int cur_frame, std::string animation="") const;
    // find last joint with anim_to in list (blank filter not allowed)
    Joint get_last_joint(int cur_frame, std::vector<std::string> animation) const;

    // adds new spritesheet coordinates as new frame from spritesheet #i
    void addFrame(IntRect rect, int i=0);
    // return frame coordinates of n-th frame
    const IntRect& getFrame(std::size_t n) const;
    // return spritesheet of n-th frame
    std::shared_ptr<Texture> getTexture(std::size_t n) const;
    // return spritesheet index of n frame
    int getTextureIndex(std::size_t n) const;
    
    // loads animation from spritesheet with specified frame size and number
    void load_from_texture(std::shared_ptr<ResourceLoader> resload, std::string name, Vector2i frame_size, int frame_count);
};

#endif // ANIMATION_INCLUDE
