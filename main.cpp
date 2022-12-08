#include <SFML/Graphics.hpp>
#include <iostream>

#include <conio.h>
#include <map>
#include <cassert>
#include <filesystem>

#include "extra_algorithms.h"

#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "Scene.h"
#include "Scene_Field.h"
#include "Scene_editor.h"
#include "UI_element.h"
#include "UI_label.h"
#include "UI_button.h"

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

int main()
{
    cout << direction_x[0] << " dir x\n";


    // setup window
    Vector2i screenDimensions(1920, 1080);
    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);

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

/********************************************************************************************/
    map <string, Texture*> UI_block;

    std::string inputPath = "Images/UI/";

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
        UI_block.insert({name, cur_texture});
    }
    cout << endl;

/********************************************************************************************/
    map <string, Texture*> field_block;

///-------------------------------------------------------------------------------------------

    /*std::string*/ inputPath = "Images/CELLS/";

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
    cout << endl;

///-------------------------------------------------------------------------------------------

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
    cout << endl;


///-------------------------------------------------------------------------------------------

    vector <UI_element*> main_ui_elements;
    {
        IntRect m1;
        m1.left = 0;
        m1.top = 0;
        Vector2u v = UI_block["horizontal_column"]->getSize();
        m1.width = v.x;
        m1.height = v.y;
        UI_element* element1 = new UI_element("horizontal", m1, UI_block["horizontal_column"]);
        main_ui_elements.push_back(element1);
    }

///-------------------------------------------------------------------------------------------
    cout << endl;

    Clock frameClock;

    Scene main_menu = new_menu_scene(&menu_texture, &new_button_texture, &new_button_pushed_texture, screenDimensions);
        main_menu.addButton("ESCAPE" ,UI_block["ESCAPE"], UI_block["ESCAPE_pushed"], 1870, 30);
    ///--------------------------------------------------------
    Scene_Field field_scene = new_field_scene(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 0);
        field_scene.someTextures(0);
        field_scene.add_Field(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 1);
    ///--------------------------------------------------------
    Scene_editor editor_scene = new_editor_scene(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 0);
        editor_scene.someTextures(0);
        editor_scene.add_Field(&field_bg_texture, 20, 20, &field_block, &player_texture, screenDimensions, 1);
        editor_scene.addButton("main_menu" ,UI_block["ESCAPE"], UI_block["ESCAPE_pushed"], 1870, 30);
        editor_scene.addUI_element(main_ui_elements);


    cout << "field made\n";
//    cout << "field cell 4 4 mapsize is " << field_scene.mapsize(4, 4) << endl;

    Scene* Current_Scene = &main_menu;

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
            std::string command_main = "";
            Current_Scene->update(event, command_main);
            if (command_main.size() > 0)
            {
                if (command_main == "editor_scene")
                {
                    Current_Scene = &editor_scene;
                }
                else if (command_main == "ESCAPE")
                {
                    window.close();
                }
                else if (command_main == "field_scene")
                {
                    Current_Scene = &field_scene;
                }
                else if (command_main == "main_menu")
                {
                    Current_Scene = &main_menu;
                }
            }
        }

        Time frameTime = frameClock.restart();

        Current_Scene->update(frameTime);


        window.clear();
        window.draw(*Current_Scene);

        window.display();

    }

    return 0;
}
