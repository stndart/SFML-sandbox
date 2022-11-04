#include <SFML/Graphics.hpp>
#include <iostream>

#include <conio.h>
#include <map>
#include <cassert>
#include <filesystem>

#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "Scene.h"
#include "Scene_Field.h"
#include "Scene_editor.h"

#include "extra_algorithms.cpp"

using namespace sf;
using namespace std;

int KeyMotion;

void Motion()
{
    switch (KeyMotion)
    {
    case 1:
        cout << "click" << endl;
        break;
    }
}

std::string re_name(std::string path)
{
    std::string result = "";
    for (unsigned int i = 0; i < path.size(); i++)
    {
        if (path[i] == '/')
        {
            result = "";
            continue;
        }
        else if (path[i] == '.')
        {
            break;
        }
        else result += path[i];
    }
    return result;
}

int main()
{
    // setup window
    Vector2i screenDimensions(1920, 1080);
    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation", sf::Style::Fullscreen);
    window.setFramerateLimit(60);

    // load textures

    Texture menu_texture;
    if (!menu_texture.loadFromFile("Images/menu.jpg"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture new_button_texture;
    if (!new_button_texture.loadFromFile("Images/new_game_button.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture new_button_pushed_texture;
    if (!new_button_pushed_texture.loadFromFile("Images/new_game_button_pushed.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture player_texture;
    if (!player_texture.loadFromFile("Images/player.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    Texture field_bg_texture;
    if (!field_bg_texture.loadFromFile("Images/field_bg.jpg"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }

    map <string, Texture*> field_block;

///-------------------------------------------------------------------------------------------

    std::string inputPath = "Images/CELLS/";

    for (auto& p : std::filesystem::directory_iterator(inputPath))
    {
        std::filesystem::path path;
        path = p;
        std::string tempStr;
        tempStr = path.generic_string();
        cout << inputPath << ": " << tempStr << ", ";

        Texture* cur_texture = new Texture;
        if (!cur_texture->loadFromFile(tempStr))
        {
            cout << "Failed to load texture\n";
            return 1;
        }
        std::string name = re_name(tempStr);
        cout << name << endl;
        field_block.insert({name, cur_texture});
    }

    inputPath = "Images/CELL_objects/";
    for (auto& p : std::filesystem::directory_iterator(inputPath))
    {
        std::filesystem::path path;
        path = p;
        std::string tempStr;
        tempStr = path.generic_string();
        cout << inputPath << ": ";

        Texture* cur_texture = new Texture;
        if (!cur_texture->loadFromFile(tempStr))
        {
            cout << "Failed to load texture\n";
            return 1;
        }
        std::string name = re_name(tempStr);
        cout << name << endl;
        field_block.insert({name, cur_texture});
    }

    //return 0;

///-------------------------------------------------------------------------------------------

    /*Texture grass_block_texture;
    if (!grass_block_texture.loadFromFile("Images/CELLS/grass.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
    field_block.insert({"grass", &grass_block_texture});

    Texture border_block_texture;
    if (!border_block_texture.loadFromFile("Images/CELLS/border.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
    field_block.insert({"border", &border_block_texture});

    Texture grass_8_block_texture;
    if (!grass_8_block_texture.loadFromFile("Images/CELLS/grass_8.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
    field_block.insert({"grass_8", &grass_8_block_texture});

    Texture null_block;
    if (!null_block.loadFromFile("Images/CELLS/null.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
    field_block.insert({"null", &null_block});

    Texture object_tree;
    if (!object_tree.loadFromFile("Images/CELL_objects/tree.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
    field_block.insert({"tree", &object_tree});

    Texture object_stump;
    if (!object_stump.loadFromFile("Images/CELL_objects/stump.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
    field_block.insert({"stump", &object_stump});
    Texture object_portal;
    if (!object_portal.loadFromFile("Images/CELL_objects/portal.png"))
    {
        cout << "Failed to load texture\n";
        return 1;
    }
    field_block.insert({"portal", &object_portal});*/

    Clock frameClock;

    Scene main_menu = new_menu_scene(&menu_texture, &new_button_texture, &new_button_pushed_texture, screenDimensions);
//    Scene_Field field_scene = new_field_scene(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 0);
//    field_scene.someTextures(0);
//    field_scene.add_Field(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 1);
    ///--------------------------------------------------------
    Scene_editor editor_scene = new_editor_scene(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 0);
    editor_scene.someTextures(0);
    editor_scene.add_Field(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 1);

    //field_scene.someTextures(1);
    cout << "field made\n";

//    cout << "field cell 4 4 mapsize is " << field_scene.mapsize(4, 4) << endl;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed){
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape){
                window.close();
            }
            //main_menu.update(event);
            //field_scene.update(event);
            editor_scene.update(event);
        }

        Time frameTime = frameClock.restart();

        //main_menu.update(frameTime);
        //field_scene.update(frameTime);
        editor_scene.update(frameTime);

        // draw
        window.clear();
        //cout << "window has been cleared" << endl;
        //window.draw(field_scene);
        window.draw(editor_scene);

        //window.draw(main_menu);
        //cout << "window has been drawn" << endl;
        window.display();
        //return 0;
    }

    return 0;
}
