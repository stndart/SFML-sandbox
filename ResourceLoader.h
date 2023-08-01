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

class Animation;

class ResourceLoader
{
private:
    // path to json config file
    std::string path_to_config;
    // flag if texture should be loaded to gpu only when requested
    bool lazy_gpu;
    // flag if texture should be loaded from disk only when requested
    bool lazy_ram;

    // default "no-texture" texture
    std::shared_ptr<Texture> notexture;
    std::shared_ptr<Image> notexture_image;
    // list of texture categories
    std::vector<std::string> categories;

    // map of all textures by categories and names
    std::map<std::string, std::map<std::string, std::shared_ptr<Texture> > > textures;
    std::map<std::string, std::map<std::string, std::shared_ptr<Image> > > tex_images;

    // map of callbacks by name
    std::map<std::string, std::function<void()> > callbacks;

    // map of animations by name. Animations loaded by Characters, but stored here, so they don't load twice
    std::map<std::string, std::shared_ptr<Animation> > animations;

    // loads subfolder contents to map
    void load_from_subfolder(std::string folder_path, std::string category, bool gpu = false, bool repeated = false);
    // load specific texture to map. Used by getting texture with lazy=true
    void load_texture_by_name(std::string texname, std::string category, bool gpu = false, bool repeated = false);

    std::shared_ptr<spdlog::logger> loading_logger;

public:
    ResourceLoader(bool lazy_ram = false, bool lazy_gpu = true, std::string path_to_config="./configs/resources.json");

    // loads all sections of textures, with paths specified in config
    // if lazy_gpu, then only loads from disk to ram (loading to gpu on request)
    void load_textures();

    // lazily returns notexture (loads from notexture_image at first call)
    std::shared_ptr<Texture> getDefaultTexture();

    // gets texture by name. By default name is texture filename without .extension
    // returns default no-texture.png if not found
    std::shared_ptr<Texture> getUITexture(std::string texname);
    std::shared_ptr<Texture> getCellTexture(std::string texname);
    std::shared_ptr<Texture> getObjectTexture(std::string texname);
    std::shared_ptr<Texture> getCharacterTexture(std::string texname);

    std::shared_ptr<Texture> getTexture(std::string texname, std::string category, bool repeated = false);

    // gets animation by name.
    std::shared_ptr<Animation> getAnimation(std::string aniname);
    void addAnimation(std::string aniname, std::shared_ptr<Animation> anim);
};

#endif // RESOURCE_LOADER_H
