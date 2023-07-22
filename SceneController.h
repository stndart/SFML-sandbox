#ifndef SCENE_CONTROLLER_H_INCLUDED
#define SCENE_CONTROLLER_H_INCLUDED

#include <string>
#include <map>
#include <memory>

#include "Scene.h"

// stores all Scenes and swithces through them
class SceneController
{
private:
    std::map<std::string, std::shared_ptr<Scene> > scene_map;

    // current scene name and pointer
    std::string cur_scene_name;

    std::shared_ptr<spdlog::logger> loading_logger;

public:
    SceneController();

    // adds scene to map
    void add_scene(std::string name, std::shared_ptr<Scene> scene);
    // switches current scene
    void set_current_scene(std::string name);

    // gets scene from map
    Scene* get_current_scene() const;
};

#endif // SCENE_CONTROLLER_H_INCLUDED
