#include "Scene.h"
#include "SceneController.h"

SceneController::SceneController(ResourceLoader* resload) : resource_manager(resload)
{
    // Reaching out to global "loading" logger by names
    loading_logger = spdlog::get("loading");

    cur_scene_name = "";
}

// reloads all scenes according to config
void SceneController::load_config(std::string path_to_config)
{
    std::ifstream f(path_to_config);
    nlohmann::json j = nlohmann::json::parse(f);

    screensize = sf::Vector2u(
        j.at("screensize")[0].get<unsigned int>(),
        j.at("screensize")[1].get<unsigned int>()
    );
    
    for (nlohmann::json it : j)
    {
        std::string scene_type = it.value("type", "Scene");
        std::string scene_name = it["name"];
        if (scene_type == "Scene")
        {
            scene_map[scene_name] = std::make_shared<Scene>(scene_name, screensize);
            scene_map[scene_name]->set_scene_controller(*this);
        }
        else if (scene_type == "Scene_Field")
        {
            scene_map[scene_name] = std::make_shared<Scene_Field>(scene_name, screensize, resource_manager);
            scene_map[scene_name]->set_scene_controller(*this);
        }
        else if (scene_type == "Scene_editor")
        {
            scene_map[scene_name] = std::make_shared<Scene_editor>(scene_name, screensize, resource_manager);
            scene_map[scene_name]->set_scene_controller(*this);
        }
        else
        {
            loading_logger->error("Not recognized Scene type: {}", scene_type);
        }
    }
}

// saves all scenes to config
void SceneController::save_config(std::string path_to_config)
{
    nlohmann::json j;
    j["screensize"] = {screensize.x, screensize.y};
    
    j["scenes"] = nlohmann::json::array();
    for (auto scene : scene_map)
    {
        j["scenes"].push_back(nlohmann::json::object());
        j["scenes"].back()["name"] = scene.first;
        j["scenes"].back()["type"] = scene.second->get_type();
        j["scenes"].back()["config"] = scene.second->get_config();
    }

    std::ofstream o(path_to_config);
    o << std::setw(4) << j << std::endl;
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

    if (scene_map[cur_scene_name]->has_callbacks())
    {
        loading_logger->warn("Discarding callbacks due to scene change");
    }

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

