#ifndef SCENE_INCLUDE
#define SCENE_INCLUDE

#include <vector>
#include "AnimatedSprite.h"

class Scene
{
private:
    std::vector<AnimatedSprite*> sprites;
    const Texture* background;
public:
};


#endif // SCENE_INCLUDE
