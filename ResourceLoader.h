#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include <memory>
#include <string>
#include <filesystem>
#include <map>

#include "extra_algorithms.h"
#include <SFML/Graphics/RenderTexture.hpp>

#include <jsoncpp/json.h>
#include <spdlog/spdlog.h>
#include <SFML/Graphics/Texture.hpp>

using namespace sf;

// adds a border to the left and bottom of the texture and replaces texture with new
void add_border(Texture* &tex, Vector2u border);
// same but with repeated texture
void add_border2(Texture* &tex, Vector2u border);

class ResourceLoader
{
private:
    std::shared_ptr<spdlog::logger> loading_logger;

public:
    Texture menu_texture;
    Texture new_button_texture;
    Texture new_button_pushed_texture;
    Texture player_texture;
    Texture field_bg_texture;

    std::map<std::string, Texture*> UI_block;

    std::map<std::string, Texture*> field_tex_map;

public:
    ResourceLoader();

    void load_main_menu_textures();
    void load_ui_textures(std::string inputPath = "Images/UI/");
    void load_cell_textures(std::string inputPath = "Images/CELLS/");
    void load_cell_object_textures(std::string inputPath = "Images/CELL_objects/");
};

#endif // RESOURCE_LOADER_H
