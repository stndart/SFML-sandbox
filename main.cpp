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

// Trash
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
    // Window initial setup: resolution, name, fulscreen, fps
    Vector2i screenDimensions(1920, 1080);
    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation", sf::Style::Fullscreen);
    window.setFramerateLimit(60);
    // If key is continuously pressed, KeyPressed event shouldn't occur multiple times
    window.setKeyRepeatEnabled(false);

    // load textures
    // TODO: move to resource loader
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///--------------------------------------= TEST downloading =--------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    map <string, Texture*> test_images;

    {
        std::string inputPath = "Images/test/";

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
            test_images.insert({name, cur_texture});
        }
        cout << endl;

    }
**/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///---------------------------------------= UI downloading =---------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    map <string, Texture*> field_tex_map;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///-------------------------------------= CELLS downloading =--------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        field_tex_map.insert({name, cur_texture});
    }
    cout << endl;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///-----------------------------------= CELL_objects downloading =---------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        field_tex_map.insert({name, cur_texture});
    }
    cout << endl;


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///----------------------------------------= UI_elements =-----------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    vector <UI_element*> main_ui_elements;
    {
        IntRect m1;
        m1.left = 0;
        m1.top = 0;
        Vector2u v1 = UI_block["horizontal_column"]->getSize();
        m1.width = v1.x;
        m1.height = v1.y;
        UI_element* element1 = new UI_element("horizontal", m1, UI_block["horizontal_column"]);
        main_ui_elements.push_back(element1);

        IntRect m2;
        m2.left = 0;
        m2.top = 1060;
        Vector2u v2 = UI_block["horizontal_column"]->getSize();
        m2.width = v2.x;
        m2.height = v2.y;
        UI_element* element2 = new UI_element("horizontal", m2, UI_block["horizontal_column"]);
        main_ui_elements.push_back(element2);

        IntRect m3;
        m3.left = 0;
        m3.top = 0;
        Vector2u v3 = UI_block["vertical_column"]->getSize();
        m3.width = v3.x;
        m3.height = v3.y;
        UI_element* element3 = new UI_element("vertical", m3, UI_block["vertical_column"]);
        main_ui_elements.push_back(element3);

        IntRect m4;
        m4.left = 1900;
        m4.top = 0;
        Vector2u v4 = UI_block["vertical_column"]->getSize();
        m4.width = v4.x;
        m4.height = v4.y;
        UI_element* element4 = new UI_element("vertical", m4, UI_block["vertical_column"]);
        main_ui_elements.push_back(element4);

//////////////////////////////////////////////////////////////////

        IntRect mb1;
        mb1.left = 1820;
        mb1.top = 0;
        Vector2u vb1 = UI_block["ESCAPE"]->getSize();
        mb1.width = vb1.x;
        mb1.height = vb1.y;

        Button* b1 = new Button("main_menu", UI_block["ESCAPE"], UI_block["ESCAPE_pushed"]);
        UI_button* elementb1 = new UI_button("main_menu", mb1, UI_block["ESCAPE_null"], b1);
        main_ui_elements.push_back(elementb1);
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    // load textures finished

    // initialize main timer (to measure fps, schedule events, etc.)
    Clock frameClock;

    // Create main menu scene. Scene is drawable and calls all subsequent draws for children drawable elements
    // We pass texture pointers: to be removed, scene must load all necessary textures via resourceloader
    Scene main_menu = new_menu_scene(&menu_texture, &new_button_texture, &new_button_pushed_texture, screenDimensions);
    // Add button with text to desired position. Textures are passed with name->texture* map "UI_block"
    main_menu.addButton("ESCAPE", UI_block["ESCAPE"], UI_block["ESCAPE_pushed"], 1820, 0);

    ///--------------------------------------------------------
    // Create field scene. At first it is inactive
    Scene_Field field_scene = Scene_Field(std::string("field_scene"), &field_tex_map);

    // current ready animations
    vector<string> player_animation_fnames =
    {
        "Images/Flametail/idle_animation_0.png",
        "Images/Flametail/idle_animation_2.png",
        "Images/Flametail/movement_0.png",
        "Images/Flametail/movement_2.png"
    };

    std::cout << "Loading fields...\n";
    // Create field with map#1. Despite it being inactive, we load map and player
    Field* field_0 = new Field(20, 20, "field_scene 0", &field_bg_texture, screenDimensions);
    field_0->load_field(field_tex_map, 0);
//    field_0->addPlayer(&player_texture, pos = player_default_pos);
    field_0->addPlayer(player_animation_fnames);

    // place_characters sets position of all dynamic sprites on field and updates view position (player in center)
    field_0->place_characters();
    field_scene.add_field(field_0, 0);

    // Create field with map#2.
    Field* field_1 = new Field(20, 20, "field_scene 1", &field_bg_texture, screenDimensions);
    field_1->load_field(field_tex_map, 1);
    //field_1->addPlayer(&player_texture, pos = player_default_pos);
    field_1->addPlayer(player_animation_fnames);
    field_1->place_characters();
    field_scene.add_field(field_1, 1);

    // Create editor scene. At first it is inactive. Scenes are currently changed via command_main switch in main loop
    Scene_editor editor_scene = Scene_editor(std::string("editor_scene"), &field_tex_map);

    // Create field with map#1 in editor scene
    Field* field_3 = new Field(20, 20, "field_scene 0", &field_bg_texture, screenDimensions);
    field_3->load_field(field_tex_map, 0);
//    field_3->addPlayer(&player_texture, pos = player_default_pos);
    field_3->addPlayer(player_animation_fnames);
    field_3->place_characters();
    editor_scene.add_field(field_3, 0);

    // Create field with map#2 in editor scene
    Field* field_4 = new Field(20, 20, "field_scene 1", &field_bg_texture, screenDimensions);
    field_4->load_field(field_tex_map, 1);
//    field_4->addPlayer(&player_texture, pos = player_default_pos);
    field_4->addPlayer(player_animation_fnames);
    field_4->place_characters();
    editor_scene.add_field(field_4, 1);

    //editor_scene.addButton("main_menu", UI_block["ESCAPE"], UI_block["ESCAPE_pushed"], 1820, 0);
    editor_scene.addUI_element(main_ui_elements);

    cout << "field made\n";

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///----------------------------------------= START programm =--------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Set default scene. It is displayed first
    Scene* Current_Scene = &main_menu;

    // main loop
    while (window.isOpen())
    {
        // Handle events loop
        Event event;
        while (window.pollEvent(event))
        {
            // Occures when x-mark in the corner of the window is pressed
            if (event.type == Event::Closed){
                window.close();
            }
            // Processes all keyboard keys
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape){
                window.close();
            }
            // Treats commands got from current scene.
            // Commands examples:
            //  ESCAPE = window close
            //  field_scene / editor_scene / main_menu = change current scene to #
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

        // get time spent per last frame and update all drawables with it
        Time frameTime = frameClock.restart();

//        std::cout << "! update in\n";
        Current_Scene->update(frameTime);
//        std::cout << "! update out\n";

        // clear previous frame and draw from scratch
        window.clear();
        window.draw(*Current_Scene);

        window.display();

    }

    return 0;
}
