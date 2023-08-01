
#include "ResourceLoader.h"

ResourceLoader::ResourceLoader(bool lazy_ram, bool lazy_gpu, std::string path_to_config) : path_to_config(path_to_config), lazy_ram(lazy_ram), lazy_gpu(lazy_gpu)
{
    // Reaching out to global "loading" logger by name
    loading_logger = spdlog::get("loading");

    notexture_image = std::make_shared<Image>();
    notexture_image->create(100, 100, sf::Color::Magenta);

    if (!lazy_ram)
        load_textures();
}

// loads subfolder contents to map
void ResourceLoader::load_from_subfolder(std::string folder_path, std::string category, bool gpu, bool repeated)
{
    int tex_counter = 0;
    for (auto& p : std::filesystem::directory_iterator(folder_path))
    {
        std::string path = static_cast<std::filesystem::path>(p).generic_string();

        std::shared_ptr<Image> cur_image = std::make_shared<Image>();
        if (!cur_image->loadFromFile(path))
        {
            loading_logger->warn("Failed to load image {}", path);
        }
        else
        {
            tex_counter++;
            std::string name = re_name(path);
            tex_images[category].insert({name, cur_image});
            
            if (gpu)
            {
                std::shared_ptr<Texture> cur_tex = std::make_shared<Texture>();
                cur_tex->setRepeated(repeated);
                cur_tex->loadFromImage(*cur_image);
                textures[category].insert({name, cur_tex});
            }

            Vector2f texsize(tex_images[category][name]->getSize());
            loading_logger->trace("Loaded image from {} to {} under name {} with size {}x{}", folder_path, path, name, texsize.x, texsize.y);
        }
    }

    if (gpu)
        loading_logger->info("Loaded {} textures to gpu", tex_counter);
    else
        loading_logger->info("Loaded {} images from disk", tex_counter);
}

// load specific texture to category
void ResourceLoader::load_texture_by_name(std::string texname, std::string category, bool gpu, bool repeated)
{
    std::ifstream f(path_to_config);
    nlohmann::json j = nlohmann::json::parse(f);

    if (!j.contains(category))
    {
        loading_logger->warn("Trying to get texture {} of unknown category {}", texname, category);
        return;
    }

    for (std::string folder : j[category])
    {
        std::string pattern = folder + texname + ".*";
        for (std::filesystem::path fpath : glob::glob(pattern))
        {
            std::string path = fpath.generic_string();

            std::shared_ptr<Image> cur_image = std::make_shared<Image>();
            if (!cur_image->loadFromFile(path))
            {
                loading_logger->warn("Failed to load image {}", path);
            }
            else
            {
                tex_images[category].insert({texname, cur_image});

                if (gpu)
                {
                    std::shared_ptr<Texture> cur_tex = std::make_shared<Texture>();
                    cur_tex->setRepeated(repeated);
                    cur_tex->loadFromImage(*cur_image);
                    textures[category].insert({texname, cur_tex});

                    loading_logger->trace("Loaded texture from {} to {} under name {} to gpu", folder, path, texname);
                }
                else
                    loading_logger->trace("Loaded image from {} to {} under name {} from disk", folder, path, texname);
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
    notexture_image = std::make_shared<Image>();
    if (!notexture_image->loadFromFile(notexture_path))
    {
        loading_logger->error("Failed to load default texture {}", notexture_path);
    }
    if (!lazy_gpu)
    {
        notexture = std::make_shared<Texture>();
        notexture->loadFromImage(*notexture_image);
    }

    categories = j["categories"];
    loading_logger->info("Loaded {} categories from {}", categories.size(), path_to_config);

    for (std::string category : categories)
    {
        loading_logger->info("Loading category {}", category);
        for (std::string folder : j[category])
        {
            loading_logger->debug("Loading folder {}", folder);
            load_from_subfolder(folder, category, !lazy_gpu, (category == "Cell") ? true : false);
        }
    }
}

std::shared_ptr<Texture> ResourceLoader::getDefaultTexture()
{
    if (notexture)
        return notexture;
    
    notexture = std::make_shared<Texture>();
    notexture->loadFromImage(*notexture_image);

    return notexture;
}

// gets texture by name. By default name is texture filename without .extension
// returns default no-texture.png if not found
std::shared_ptr<Texture> ResourceLoader::getTexture(std::string texname, std::string category, bool repeated)
{
    // category may be unknown only if config is parsed
    if (!lazy_ram && tex_images.count(category) == 0)
    {
        loading_logger->warn("Trying to get texture {} of unknown category {}", texname, category);
        return getDefaultTexture();
    }
    else
    {
        // if texture is not loaded
        if (textures[category].count(texname) == 0)
        {
            // lazy_gpu -> check if it is in ram
            // lazy_ram -> load to ram (and to gpu)
            
            // texture is in ram, but not on gpu (because lazy)
            if (lazy_gpu && tex_images[category].count(texname) != 0)
            {
                textures[category][texname] = std::make_shared<Texture>();
                textures[category][texname]->loadFromImage(*tex_images[category][texname]);
                textures[category][texname]->setRepeated(repeated);
                return textures[category][texname];
            }
            // texture is not in ram, because lazy, then try to load it
            if (lazy_ram)
            {
                load_texture_by_name(texname, category, true, repeated);
            }
            // unable to load texture
            if (textures[category].count(texname) == 0)
            {
                loading_logger->warn("Trying to get unknown texture {} of category {}", texname, category);
                return getDefaultTexture();
            }
            return textures[category][texname];
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
    return getTexture(texname, "Cell", true);
}

std::shared_ptr<Texture> ResourceLoader::getObjectTexture(std::string texname)
{
    return getTexture(texname, "Cell object");
}

std::shared_ptr<Texture> ResourceLoader::getCharacterTexture(std::string texname)
{
    return getTexture(texname, "Character");
}

// gets animation by name. If not present, returns ptr<Animation>(nullptr)
std::shared_ptr<Animation> ResourceLoader::getAnimation(std::string aniname)
{
    return animations[aniname];
}

void ResourceLoader::addAnimation(std::string aniname, std::shared_ptr<Animation> anim)
{
    animations[aniname] = anim;
}