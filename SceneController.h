#ifndef SCENE_CONTROLLER_H_INCLUDED
#define SCENE_CONTROLLER_H_INCLUDED

#include <string>
#include <map>
#include <memory>
#include <fstream>

#include "ResourceLoader.h"
#include "Scene.h"
#include "Player.h"
#include "Scene_Field.h"
#include "Scene_editor.h"

#include <nlohmann/json.hpp>

// SceneController contains and manages all Scenes.
// Change current scene with set_current_scene. While changing scene, SceneController discards any unexecuted callbacks of old scene.

class SceneController
{
private:
    // map, where Scenes are stored
    std::map<std::string, std::shared_ptr<Scene> > scene_map;

    std::shared_ptr<sf::RenderWindow> current_window;

    // current scene name and pointer
    std::string cur_scene_name;

    std::shared_ptr<ResourceLoader> resource_manager;

    std::shared_ptr<spdlog::logger> loading_logger;

public:
    SceneController(std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<ResourceLoader> resload = std::make_shared<ResourceLoader>(nullptr));

    // reloads all scenes according to config
    void load_config(std::string path_to_config = "./configs/scenes.json");
    // saves all scenes to config
    void save_config(std::string path_to_config);

    // adds scene to map
    void add_scene(std::string name, std::shared_ptr<Scene> scene);
    // switches current scene
    void set_current_scene(std::string name);

    // gets scene by name
    std::shared_ptr<Scene> get_scene(std::string name) const;
    // gets scene from map
    std::shared_ptr<Scene> get_current_scene() const;
    // gets current window
    std::shared_ptr<sf::RenderWindow> get_current_window() const;
};

#endif // SCENE_CONTROLLER_H_INCLUDED
