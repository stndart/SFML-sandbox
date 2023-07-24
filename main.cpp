#include <conio.h>
#include <map>
#include <cassert>
#include <filesystem>
#include <string>

// for logging, printing, and make_shared
#include <iostream>
#include <memory>

// logging library
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "Scene.h"
#include "Scene_Field.h"
#include "Scene_editor.h"
#include "SceneController.h"
#include "extra_algorithms.h"

#include <SFML/Graphics.hpp>

//#include "UI_element.h"
//#include "UI_button.h"

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
        // truncate = true - means delete log on startup
        auto logfile_sink = std::make_shared<spdlog::sinks::basic_file_sink_st>(logfile_path, true);
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_sink_st>();
        spdlog::sinks_init_list sink_list = {logfile_sink, stdout_sink};
        // create synchronous loggers
        loading_logger = std::make_shared<spdlog::logger>("loading", sink_list.begin(), sink_list.end());
        input_logger = std::make_shared<spdlog::logger>("input", sink_list.begin(), sink_list.end());
        map_events_logger = std::make_shared<spdlog::logger>("map_events", sink_list.begin(), sink_list.end());
        graphics_logger = std::make_shared<spdlog::logger>("graphics", sink_list.begin(), sink_list.end());

        loading_logger->set_level(spdlog::level::info);
        input_logger->set_level(spdlog::level::info);
        map_events_logger->set_level(spdlog::level::info);
        graphics_logger->set_level(spdlog::level::info);

        loading_logger->flush_on(spdlog::level::trace);
        input_logger->flush_on(spdlog::level::trace);
        map_events_logger->flush_on(spdlog::level::trace);
        graphics_logger->flush_on(spdlog::level::trace);

        stdout_sink->set_level(spdlog::level::debug);
        logfile_sink->set_level(spdlog::level::trace);

        // globally register the loggers so they can be accessed using spdlog::get(logger_name)
        spdlog::register_logger(loading_logger);
        spdlog::register_logger(input_logger);
        spdlog::register_logger(map_events_logger);
        spdlog::register_logger(graphics_logger);

        /*
        loading_logger - логгер загрузки ресурсов (поля, текстур)
        input_logger - логгер ввода с клавиатуры/мышки, и сетевого (мб потом будет отдельно)
        map_events_logger - логгер изменения объектов на карте, включая игрока и entities
        graphics_logger - логгер событий отрисовки, в том числе анимаций и их логики */
    }
    catch (const spdlog::spdlog_ex& ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
        return 1;
    }

    /// TODO: try-catch (lookup error type)
    // Window initial setup: resolution, name, fulscreen, fps
    Vector2i screenDimensions(1920, 1080);
    shared_ptr<RenderWindow> window = make_shared<RenderWindow>(VideoMode(screenDimensions.x, screenDimensions.y), "Animation", sf::Style::Fullscreen);
//    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation");
    window->setFramerateLimit(60);
    // If key is continuously pressed, KeyPressed event shouldn't occur multiple times
    window->setKeyRepeatEnabled(false);
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
        Animation tAn(UI_block["horizontal_column"]);
        tAn.addFrame(m1, 0);
        //UI_button* element1 = new UI_button("horizontal", m1, &tAn);
        //main_ui_elements.push_back(element1);

        IntRect m2;
        m2.left = 0;
        m2.top = 1060;
        Vector2u v2 = UI_block["horizontal_column"]->getSize();
        m2.width = v2.x;
        m2.height = v2.y;
        //UI_button* element2 = new UI_button("horizontal", m2, &tAn);
        //main_ui_elements.push_back(element2);

        IntRect m3;
        m3.left = 0;
        m3.top = 0;
        Vector2u v3 = UI_block["vertical_column"]->getSize();
        m3.width = v3.x;
        m3.height = v3.y;
        Animation tAn2(UI_block["vertical_column"]);
        tAn2.addFrame(m3, 0);
        //UI_button* element3 = new UI_button("vertical", m3, &tAn2);
        //main_ui_elements.push_back(element3);

        IntRect m4;
        m4.left = 1900;
        m4.top = 0;
        Vector2u v4 = UI_block["vertical_column"]->getSize();
        m4.width = v4.x;
        m4.height = v4.y;
        //UI_button* element4 = new UI_button("vertical", m4, &tAn2);
        //main_ui_elements.push_back(element4);

//////////////////////////////////////////////////////////////////

        IntRect mb1(1820, 0, 0, 0);
        Vector2u vb1 = UI_block["ESCAPE"]->getSize();
        mb1.width = vb1.x;
        mb1.height = vb1.y;

        Animation tAnb;
        tAnb.addSpriteSheet(UI_block["ESCAPE"]);
        tAnb.addFrame(mb1, 0);
        tAnb.addSpriteSheet(UI_block["ESCAPE_pushed"]);
        tAnb.addFrame(mb1, 1);
        //UI_button* elementb1 = new UI_button("main_menu_button", mb1, &tAnb);
        //main_ui_elements.push_back(elementb1);
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    // load textures finished

    // initialize main timer (to measure fps, schedule events, etc.)
    Clock frameClock;

    // Create main menu scene. Scene is drawable and calls all subsequent draws for children drawable elements
    // We pass texture pointers: to be removed, scene must load all necessary textures via resourceloader through config json
    shared_ptr<Scene> main_menu = new_menu_scene(&menu_texture, &new_button_texture, &new_button_pushed_texture, screenDimensions);
    // Add button with text to desired position. Textures are passed via name->texture* map "UI_block"
    main_menu->addButton("ESCAPE", UI_block["ESCAPE"], UI_block["ESCAPE_pushed"], 1820, 0, create_window_closed_callback(window), "top left");

    // Create field scene. At first it is inactive. Name and textures are passed
    shared_ptr<Scene_Field> field_scene = std::make_shared<Scene_Field>(std::string("field_scene"), &field_tex_map);

    // Create editor scene. At first it is inactive. Scenes are swapped with callbacks to SceneController
    shared_ptr<Scene_editor> editor_scene = std::make_shared<Scene_editor>(std::string("editor_scene"), &field_tex_map);

    // Set default scene. It is displayed first
    SceneController scene_controller;
    scene_controller.add_scene("main_menu", main_menu);
    scene_controller.add_scene("Scene_editor", editor_scene);
    scene_controller.add_scene("Scene_field", field_scene);

    // current ready animations
    // each animation are loaded x4, because no resourceloader
    vector<string> player_animation_fnames =
    {
        "Images/Flametail/idle_animation_0.png",
        "Images/Flametail/idle_animation_2.png",
        "Images/Flametail/movement_0.png",
        "Images/Flametail/movement_2.png"
    };

    // Create field with map#1. Despite it being inactive, we load map and player
    Field* field_0 = new Field("field_scene 0", &field_bg_texture, screenDimensions);
    field_0->load_field(field_tex_map, 0);
    // Create first and only player.
    // Player knows, where he stands, and the field also
    field_0->addPlayer(player_animation_fnames);
    shared_ptr<Player> player_0 = field_0->player_0;

    // place_characters sets position of all dynamic sprites on field and updates view position (player in center)
    field_0->place_characters();
    field_scene->add_field(field_0, 0);

    // Create field with map#2.
    Field* field_1 = new Field("field_scene 1", &field_bg_texture, screenDimensions);
    field_1->load_field(field_tex_map, 1);
    field_1->player_0 = player_0;
    field_scene->add_field(field_1, 1);

    // Create field with map#1 in editor scene
    Field* field_3 = new Field("field_scene 0", &field_bg_texture, screenDimensions);
    field_3->load_field(field_tex_map, 0);
    field_3->player_0 = player_0;
    editor_scene->add_field(field_3, 0);

    // because whould be the first displayed field
    player_0->set_current_field(field_3);

    // Create field with map#2 in editor scene
    Field* field_4 = new Field("field_scene 1", &field_bg_texture, screenDimensions);
    field_4->load_field(field_tex_map, 1);
    field_4->player_0 = player_0;
    editor_scene->add_field(field_4, 1);

    editor_scene->addButton("to_main_menu", UI_block["ESCAPE"], UI_block["ESCAPE_pushed"], 1820, 0, create_change_scene_callback(editor_scene, "main_menu"), "top left");
//    editor_scene->addButton("bloodscreen", UI_block["BLOOD"], UI_block["BLOOD"], IntRect(1620, 0, 200, 100), create_bloodscreen(editor_scene, Color(0, 0, 0, 100)), "top left");
    editor_scene->addButton("bloodscreen", UI_block["BLOOD"], UI_block["BLOOD"], IntRect(1620, 0, 200, 100), create_light_circle(editor_scene), "top left");
    editor_scene->addButton("clear blood", UI_block["BLOOD"], UI_block["BLOOD"], IntRect(1420, 0, 200, 100), clear_bloodscreen(editor_scene), "top left");
    editor_scene->addUI_element(main_ui_elements);

    loading_logger->info("Loaded fields");

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///----------------------------------------= START programm =--------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Well, because we start with main menu
    scene_controller.set_current_scene("main_menu");

    // main loop
    while (window->isOpen())
    {
        // Handle events loop
        Event event;
        Scene* cur_scene = scene_controller.get_current_scene();

        while (window->pollEvent(event))
        {
            // Occures when x-mark in the corner of the window is pressed
            // TODO: research, which system events exist and when occur
            if (event.type == Event::Closed){
                window->close();
            }
            // Processes all input (and not only) events
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape){
                window->close();
            }
            // Treats commands got from current scene.
            // Commands examples:
            //  ESCAPE = window close
            //  field_scene / editor_scene / main_menu = change current scene to #
            std::string command_main = "";
            if (cur_scene)
                cur_scene->update(event, command_main);
        }

        // get time spent per last frame and update all drawables with it
        Time frameTime = frameClock.restart();
        if (cur_scene)
            cur_scene->update(frameTime);

        // clear previous frame and draw from scratch
        window->clear();
        if (cur_scene)
            window->draw(*cur_scene);

        window->display();

    }

    return 0;
}
