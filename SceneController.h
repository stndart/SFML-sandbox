#ifndef SCENE_CONTROLLER_H_INCLUDED
#define SCENE_CONTROLLER_H_INCLUDED

#include <string>
#include <map>
#include <memory>
#include <fstream>

#include "Scene.h"
#include "Player.h"
#include "Scene_Field.h"
#include "Scene_editor.h"
#include "ResourceLoader.h"

#include <nlohmann/json.hpp>

// SceneController contains and manages all Scenes.
// Change current scene with set_current_scene. While changing scene, SceneController discards any unexecuted callbacks of old scene.

class SceneController
{
private:
    // map, where Scenes are stored
    std::map<std::string, std::shared_ptr<Scene> > scene_map;

    // current scene name and pointer
    std::string cur_scene_name;
    // screensize is loaded from config json
    sf::Vector2u screensize;

    std::shared_ptr<spdlog::logger> loading_logger;

    ResourceLoader* resource_manager;

public:
    SceneController(ResourceLoader* resload = nullptr);

    // reloads all scenes according to config
    void load_config(std::string path_to_config = "./configs/scenes.json");
    // saves all scenes to config
    void save_config(std::string path_to_config);

    // adds scene to map
    void add_scene(std::string name, std::shared_ptr<Scene> scene);
    // switches current scene
    void set_current_scene(std::string name);

    // gets scene from map
    Scene* get_current_scene() const;
};

#endif // SCENE_CONTROLLER_H_INCLUDED
