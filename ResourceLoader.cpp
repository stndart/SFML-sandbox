
#include "ResourceLoader.h"

ResourceLoader::ResourceLoader(std::string path_to_config, bool lazy) : path_to_config(path_to_config), lazy(lazy)
{
    // Reaching out to global "loading" logger by name
    loading_logger = spdlog::get("loading");

    notexture = std::make_shared<Texture>();
    notexture->create(100, 100);

    if (!lazy)
        load_textures();
}

// loads subfolder contents to map
void ResourceLoader::load_from_subfolder(std::string folder_path, std::map<std::string, std::shared_ptr<Texture> >& load_to, bool repeated)
{
    int tex_counter = 0;
    for (auto& p : std::filesystem::directory_iterator(folder_path))
    {
        std::string path = static_cast<std::filesystem::path>(p).generic_string();

        std::shared_ptr<Texture> cur_texture = std::make_shared<Texture>();
        cur_texture->setRepeated(repeated);
        if (!cur_texture->loadFromFile(path))
        {
            loading_logger->warn("Failed to load texture {}", path);
        }
        else
        {
            tex_counter++;
            std::string name = re_name(path);
            load_to.insert({name, cur_texture});

            loading_logger->trace("Loaded texture from {} to {} under name {}", folder_path, path, name);
        }
    }
    loading_logger->info("Loaded {} textures", tex_counter);
}

// load specific texture to category
void ResourceLoader::load_texture_by_name(std::string texname, std::string category)
{
    std::ifstream f(path_to_config);
    nlohmann::json j = nlohmann::json::parse(f);

    for (std::string folder : j[category])
    {
        std::string pattern = folder + texname + ".*";
        for (std::filesystem::path fpath : glob::glob(pattern))
        {
            std::string path = fpath.generic_string();

            std::shared_ptr<Texture> cur_texture = std::make_shared<Texture>();
            cur_texture->setRepeated((category == "Cell") ? false : true);
            if (!cur_texture->loadFromFile(path))
            {
                loading_logger->warn("Failed to load texture {}", path);
            }
            else
            {
                textures[category].insert({texname, cur_texture});
                loading_logger->trace("Loaded texture from {} to {} under name {}", folder, path, texname);
            }
        }
    }
}

// loads all sections of textures, with paths specified in config
void ResourceLoader::load_textures()
{
    std::ifstream f(path_to_config);
    nlohmann::json j = nlohmann::json::parse(f);

    std::string notexture_path = j["default"].get<std::string>();
    notexture = std::make_shared<Texture>();
    if (!notexture->loadFromFile(notexture_path))
    {
        loading_logger->error("Failed to load default texture {}", notexture_path);
    }

    categories = j["categories"];
    loading_logger->info("Loaded {} categories from {}", categories.size(), path_to_config);

    for (std::string category : categories)
    {
        loading_logger->info("Loading category {}", category);
        for (std::string folder : j[category])
        {
            loading_logger->debug("Loading folder {}", folder);
            load_from_subfolder(folder, textures[category], (category == "Cell") ? true : false);
        }
    }
}

// gets texture by name. By default name is texture filename without .extension
// returns default no-texture.png if not found
std::shared_ptr<Texture> ResourceLoader::getTexture(std::string texname, std::string category)
{
    if (textures.count(category) == 0)
    {
        loading_logger->warn("Trying to get texture {} of unknown category {}", texname, category);
        return notexture;
    }
    else
    {
        if (textures[category].count(texname) == 0)
        {
            if (!lazy)
            {
                loading_logger->warn("Trying to get unknown texture {} of category {}", texname, category);
                return notexture;
            }
            else
            {
                load_texture_by_name(texname, category);
                if (textures[category].count(texname) == 0)
                    return notexture;
                else
                    return textures[category][texname];
            }
        }
        else
        {
            return textures[category][texname];
        }
    }
}

std::shared_ptr<Texture> ResourceLoader::getUITexture(std::string texname)
{
    return getTexture(texname, "UI");
}

std::shared_ptr<Texture> ResourceLoader::getCellTexture(std::string texname)
{
    return getTexture(texname, "Cell");
}

std::shared_ptr<Texture> ResourceLoader::getObjectTexture(std::string texname)
{
    return getTexture(texname, "Cell object");
}

std::shared_ptr<Texture> ResourceLoader::getCharacterTexture(std::string texname)
{
    return getTexture(texname, "Character");
}

// gets animation by name.
std::shared_ptr<Animation> ResourceLoader::getAnimation(std::string aniname)
{
    return animations[aniname];
}

void ResourceLoader::addAnimation(std::string aniname, std::shared_ptr<Animation> anim)
{
    animations[aniname] = anim;
}