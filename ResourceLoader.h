#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <memory>
#include <string>
#include <filesystem>
#include <map>
#include <fstream>

#include "extra_algorithms.h"
#include "Callbacks.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <glob/glob.h>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;

class ResourceLoader
{
private:
    // path to json config file
    std::string path_to_config;
    // flag if texture should be loaded only when requested
    bool lazy;

    // default "no-texture" texture
    std::shared_ptr<Texture> notexture;
    // list of texture categories
    std::vector<std::string> categories;
    // map of all textures by categories and names
    std::map<std::string, std::map<std::string, std::shared_ptr<Texture> > > textures;

    // map of callbacks by name
    std::map<std::string, std::function<void()> > callbacks;

    // loads subfolder contents to map
    void load_from_subfolder(std::string folder_path, std::map<std::string, std::shared_ptr<Texture> >&  load_to, bool repeated = false);
    // load specific texture to map. Used by getting texture with lazy=true
    void load_texture_by_name(std::string texname, std::string category);

    std::shared_ptr<spdlog::logger> loading_logger;

public:
    ResourceLoader(std::string path_to_config="./configs/resources.json", bool lazy=false);

    // loads all sections of textures, with paths specified in config
    void load_textures();

    // gets texture by name. By default name is texture filename without .extension
    // returns default no-texture.png if not found
    std::shared_ptr<Texture> getUITexture(std::string texname);
    std::shared_ptr<Texture> getCellTexture(std::string texname);
    std::shared_ptr<Texture> getObjectTexture(std::string texname);
    std::shared_ptr<Texture> getCharacterTexture(std::string texname);

    std::shared_ptr<Texture> getTexture(std::string texname, std::string category);
};

#endif // RESOURCE_LOADER_H
