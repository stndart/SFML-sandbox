
#include "ResourceLoader.h"

ResourceLoader::ResourceLoader()
{
    // Reaching out to global "loading" logger by name
    loading_logger = spdlog::get("loading");
}

void ResourceLoader::load_main_menu_textures()
{
    int tex_counter = 0;
    if (!menu_texture.loadFromFile("Images/menu.jpg"))
    {
        loading_logger->critical("Failed to load texture");
        throw;
    }
    tex_counter++;

    if (!new_button_texture.loadFromFile("Images/new_game_button.png"))
    {
        loading_logger->critical("Failed to load texture");
        throw;
    }
    tex_counter++;

    if (!new_button_pushed_texture.loadFromFile("Images/new_game_button_pushed.png"))
    {
        loading_logger->critical("Failed to load texture");
        throw;
    }
    tex_counter++;

    if (!player_texture.loadFromFile("Images/player.png"))
    {
        loading_logger->critical("Failed to load texture");
        throw;
    }
    tex_counter++;

    if (!field_bg_texture.loadFromFile("Images/field_bg.jpg"))
    {
        loading_logger->critical("Failed to load texture");
        throw;
    }
    tex_counter++;

    loading_logger->info("Loaded {} textures", tex_counter);
}

void ResourceLoader::load_ui_textures(std::string inputPath)
{
    int tex_counter = 0;
    for (auto& p : std::filesystem::directory_iterator(inputPath))
    {
        std::filesystem::path path;
        path = p;
        std::string tempStr;
        tempStr = path.generic_string();

        Texture* cur_texture = new Texture;
        if (!cur_texture->loadFromFile(tempStr))
        {
            loading_logger->critical("Failed to load texture");
            throw;
        }
        tex_counter++;
        std::string name = re_name(tempStr);
        loading_logger->trace("{}: {}, {}", inputPath, tempStr, name);
        UI_block.insert({name, cur_texture});
    }
    loading_logger->info("Loaded {} textures for UI", tex_counter);
}

void ResourceLoader::load_cell_textures(std::string inputPath)
{
    int tex_counter = 0;
    for (auto& p : std::filesystem::directory_iterator(inputPath))
    {
        std::filesystem::path path;
        path = p;
        std::string tempStr;
        tempStr = path.generic_string();

        Texture* cur_texture = new Texture;
        cur_texture->setRepeated(true);
        if (!cur_texture->loadFromFile(tempStr))
        {
            loading_logger->critical("Failed to load texture");
            throw;
        }
        tex_counter++;
        std::string name = re_name(tempStr);
        loading_logger->trace("{}: {}, {}", inputPath, tempStr, name);
        field_tex_map.insert({name, cur_texture});
    }
    loading_logger->info("Loaded {} textures for cells", tex_counter);
}

void ResourceLoader::load_cell_object_textures(std::string inputPath)
{
    int tex_counter = 0;
    for (auto& p : std::filesystem::directory_iterator(inputPath))
    {
        std::filesystem::path path;
        path = p;
        std::string tempStr;
        tempStr = path.generic_string();

        Texture* cur_texture = new Texture;
        if (!cur_texture->loadFromFile(tempStr))
        {
            loading_logger->critical("Failed to load texture");
            throw;
        }
        tex_counter++;
        std::string name = re_name(tempStr);
        loading_logger->trace("{}: {}, {}", inputPath, tempStr, name);
        field_tex_map.insert({name, cur_texture});
    }
    loading_logger->info("Loaded {} textures for cell objects", tex_counter);
}


