#include "Scene.h"
#include "SceneController.h"

SceneController::SceneController()
{
    // Reaching out to global "loading" logger by names
    loading_logger = spdlog::get("loading");

    cur_scene_name = "";
}

// adds scene to map
void SceneController::add_scene(std::string name, std::shared_ptr<Scene> scene)
{
    loading_logger->info("Adding scene {} to SceneController", name);

    scene_map[name] = std::move(scene);
    if (cur_scene_name == "")
        cur_scene_name = name;
    loading_logger->info("set cur_scene_name to {}", cur_scene_name);

    // sets new map with scen_contoller as self
    scene_map[name]->set_scene_controller(*this);
}

// switches current scene
void SceneController::set_current_scene(std::string name)
{
    loading_logger->info("Changing scene to {}", name);

    scene_map[cur_scene_name]->cancel_callbacks();
    cur_scene_name = name;
}

// gets scene from map
Scene* SceneController::get_current_scene() const
{
    if (cur_scene_name == "")
        return NULL;

    if (scene_map.count(cur_scene_name) == 0)
    {
        loading_logger->error("Trying to get unexistent scene \"{}\" from SceneController", cur_scene_name);
        throw;
    }

    return scene_map.at(cur_scene_name).get();
}

