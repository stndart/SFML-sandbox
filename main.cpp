#include <SFML/Graphics.hpp>

#include <conio.h>
#include <map>
#include <cassert>
#include <filesystem>

// for logging, printing, and make_shared
#include <iostream>
#include <memory>

// logging library
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"

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

int main()
{
    /**
    SPDLOG_LEVEL_TRACE - вызовы вспомогательных функций, принты в них
    SPDLOG_LEVEL_DEBUG - вызовы второстепенных рабочих функций (типа апдейтов, обработчиков событий)
    SPDLOG_LEVEL_INFO - вызовы важных функций, создание глобальных объектов
    SPDLOG_LEVEL_WARN - Переходы в урезанные режимы, включение режимов не по умолчанию (в важных объектах, типа joint_ignore у Character)
    SPDLOG_LEVEL_ERROR - Ошибки
    SPDLOG_LEVEL_CRITICAL - Критические ошибки: сегфолты, несуществующие файлы, утечки памяти, null pointer

    loading_logger - логгер загрузки ресурсов (поля, текстур)
    input_logger - логгер ввода с клавиатуры/мышки, и сетевого (мб потом будет отдельно)
    map_events_logger - логгер изменения объектов на карте, включая игрока и entities
    graphics_logger - логгер событий отрисовки, в том числе анимаций и их логики
    */

    std::shared_ptr<spdlog::logger> loading_logger, input_logger, map_events_logger, graphics_logger;

    // LOGGING
    try
    {
        // load loggers and log sinks
        string logfile_path = "logs/log.txt"; /// TEMP (to config file)
        auto logfile_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(logfile_path);
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_sink_st>();
        spdlog::sinks_init_list sink_list = {logfile_sink, stdout_sink};
        // create synchronous loggers
        loading_logger = std::make_shared<spdlog::logger>("loading", sink_list.begin(), sink_list.end());
        input_logger = std::make_shared<spdlog::logger>("input", sink_list.begin(), sink_list.end());
        map_events_logger = std::make_shared<spdlog::logger>("map_events", sink_list.begin(), sink_list.end());
        graphics_logger = std::make_shared<spdlog::logger>("graphics", sink_list.begin(), sink_list.end());

        loading_logger->set_level(spdlog::level::trace);
        input_logger->set_level(spdlog::level::info);
        map_events_logger->set_level(spdlog::level::info);
        graphics_logger->set_level(spdlog::level::info);

        stdout_sink->set_level(spdlog::level::info);
        logfile_sink->set_level(spdlog::level::debug);

        // globally register the loggers so they can be accessed using spdlog::get(logger_name)
        spdlog::register_logger(loading_logger);
        spdlog::register_logger(input_logger);
        spdlog::register_logger(map_events_logger);
        spdlog::register_logger(graphics_logger);
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    /// TODO: try-catch (lookup error type)
    // Window initial setup: resolution, name, fulscreen, fps
    Vector2i screenDimensions(1920, 1080);
    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation", sf::Style::Fullscreen);
//    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation");
    window.setFramerateLimit(60);
    // If key is continuously pressed, KeyPressed event shouldn't occur multiple times
    window.setKeyRepeatEnabled(false);
    loading_logger->info("Created window {}x{}", 1920, 1080);

    // load textures
    /// TODO: move to resource loader
    Texture menu_texture;
    int tex_counter = 0;
    if (!menu_texture.loadFromFile("Images/menu.jpg"))
    {
        loading_logger->critical("Failed to load texture");
        return 1;
    }
    tex_counter++;

    Texture new_button_texture;
    if (!new_button_texture.loadFromFile("Images/new_game_button.png"))
    {
        loading_logger->critical("Failed to load texture");
        return 1;
    }
    tex_counter++;

    Texture new_button_pushed_texture;
    if (!new_button_pushed_texture.loadFromFile("Images/new_game_button_pushed.png"))
    {
        loading_logger->critical("Failed to load texture");
        return 1;
    }
    tex_counter++;

    Texture player_texture;
    if (!player_texture.loadFromFile("Images/player.png"))
    {
        loading_logger->critical("Failed to load texture");
        return 1;
    }
    tex_counter++;

    Texture field_bg_texture;
    if (!field_bg_texture.loadFromFile("Images/field_bg.jpg"))
    {
        loading_logger->critical("Failed to load texture");
        return 1;
    }
    tex_counter++;
    loading_logger->info("Loaded {} textures", tex_counter);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
///--------------------------------------= TEST downloading =--------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    map <string, Texture*> test_images;

    {
        std::string inputPath = "Images/test/";

        tex_counter = 0;
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
                return 1;
            }
            tex_counter++;
            std::string name = re_name(tempStr);
            loading_logger->trace("{}: {}, {}", inputPath, tempStr, name);
            test_images.insert({name, cur_texture});
        }
        loading_logger->info("Loaded {} textures", tex_counter);
    }
**/
/////////////////////////////////////////////////////////////////////////////////////////////////////////
///---------------------------------------= UI downloading =---------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    map <string, Texture*> UI_block;

    std::string inputPath = "Images/UI/";

    tex_counter = 0;
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
            return 1;
        }
        tex_counter++;
        std::string name = re_name(tempStr);
        loading_logger->trace("{}: {}, {}", inputPath, tempStr, name);
        UI_block.insert({name, cur_texture});
    }
    loading_logger->info("Loaded {} textures for UI", tex_counter);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    map <string, Texture*> field_tex_map;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///-------------------------------------= CELLS downloading =--------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*std::string*/ inputPath = "Images/CELLS/";
    tex_counter = 0;
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
            return 1;
        }
        tex_counter++;
        std::string name = re_name(tempStr);
        loading_logger->trace("{}: {}, {}", inputPath, tempStr, name);
        field_tex_map.insert({name, cur_texture});
    }
    loading_logger->info("Loaded {} textures for cells", tex_counter);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///-----------------------------------= CELL_objects downloading =---------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    inputPath = "Images/CELL_objects/";
    tex_counter = 0;
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
            return 1;
        }
        tex_counter++;
        std::string name = re_name(tempStr);
        loading_logger->trace("{}: {}, {}", inputPath, tempStr, name);
        field_tex_map.insert({name, cur_texture});
    }
    loading_logger->info("Loaded {} textures for cell objects", tex_counter);


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

    loading_logger->info("Loaded fields");

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

                    map_events_logger->info("Switched scene to editor_scene");
                }
                else if (command_main == "ESCAPE")
                {
                    loading_logger->info("Closed window");

                    window.close();
                }
                else if (command_main == "field_scene")
                {
                    Current_Scene = &field_scene;

                    map_events_logger->info("Switched scene to field_scene");
                }
                else if (command_main == "main_menu")
                {
                    Current_Scene = &main_menu;

                    map_events_logger->info("Switched scene to main_menu");
                }
            }
        }

        // get time spent per last frame and update all drawables with it
        Time frameTime = frameClock.restart();

        Current_Scene->update(frameTime);

        // clear previous frame and draw from scratch
        window.clear();
        window.draw(*Current_Scene);

        window.display();

    }

    return 0;
}
