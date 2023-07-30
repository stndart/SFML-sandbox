#include <map>
#include <cassert>
#include <filesystem>
#include <string>

// for logging, printing, and make_shared
#include <iostream>
#include <memory>

// logging library
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

// dear imgui temporary library
#include <imgui.h>
#include <imgui-SFML.h>

// json parsing library
#include <nlohmann/json.hpp>

#include "AnimatedSprite.h"
#include "VisualEffect.h"
#include "Scene.h"
#include "Scene_Field.h"
#include "Scene_editor.h"
#include "SceneController.h"
#include "ResourceLoader.h"
#include "UI_button.h"
#include "extra_algorithms.h"
#include "Callbacks.h"

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

        loading_logger->set_level(spdlog::level::debug);
        input_logger->set_level(spdlog::level::info);
        map_events_logger->set_level(spdlog::level::trace);
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
    Vector2u screenDimensions(1920, 1080);
    shared_ptr<RenderWindow> window = make_shared<RenderWindow>(VideoMode(screenDimensions.x, screenDimensions.y), "Animation", sf::Style::Fullscreen);
//    RenderWindow window(VideoMode(screenDimensions.x, screenDimensions.y), "Animation");
    window->setFramerateLimit(60);
    // If key is continuously pressed, KeyPressed event shouldn't occur multiple times
    window->setKeyRepeatEnabled(false);
    loading_logger->info("Created window {}x{}", 1920, 1080);

    if (!ImGui::SFML::Init(*window))
    {
        loading_logger->error("Couldn't init imgui");
        return 1;
    }

    // load all textures, since lazy=off
    std::shared_ptr<ResourceLoader> resload = std::make_shared<ResourceLoader>();

    loading_logger->info("Resource manager texture load complete");

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///----------------------------------------= UI_elements =-----------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//     vector<shared_ptr<UI_element> > main_ui_elements;
//     {
//         IntRect m1;
//         m1.left = 0;
//         m1.top = 0;
//         Vector2u v1 = resload.UI_block["horizontal_column"]->getSize();
//         m1.width = v1.x;
//         m1.height = v1.y;
//         Animation tAn(resload.UI_block["horizontal_column"]);
//         tAn.addFrame(m1, 0);
//         //shared_ptr<UI_button> element1("horizontal", m1, &tAn);
//         //main_ui_elements.push_back(element1);

//         IntRect m2;
//         m2.left = 0;
//         m2.top = 1060;
//         Vector2u v2 = resload.UI_block["horizontal_column"]->getSize();
//         m2.width = v2.x;
//         m2.height = v2.y;
//         //shared_ptr<UI_button> element2("horizontal", m2, &tAn);
//         //main_ui_elements.push_back(element2);

//         IntRect m3;
//         m3.left = 0;
//         m3.top = 0;
//         Vector2u v3 = resload.UI_block["vertical_column"]->getSize();
//         m3.width = v3.x;
//         m3.height = v3.y;
//         Animation tAn2(resload.UI_block["vertical_column"]);
//         tAn2.addFrame(m3, 0);
//         //shared_ptr<UI_button> element3("vertical", m3, &tAn2);
//         //main_ui_elements.push_back(element3);

//         IntRect m4;
//         m4.left = 1900;
//         m4.top = 0;
//         Vector2u v4 = resload.UI_block["vertical_column"]->getSize();
//         m4.width = v4.x;
//         m4.height = v4.y;
//         //shared_ptr<UI_button> element4("vertical", m4, &tAn2);
//         //main_ui_elements.push_back(element4);

// //////////////////////////////////////////////////////////////////

//         IntRect mb1(1820, 0, 0, 0);
//         Vector2u vb1 = resload.UI_block["ESCAPE"]->getSize();
//         mb1.width = vb1.x;
//         mb1.height = vb1.y;

//         Animation tAnb;
//         tAnb.addSpriteSheet(resload.UI_block["ESCAPE"]);
//         tAnb.addFrame(mb1, 0);
//         tAnb.addSpriteSheet(resload.UI_block["ESCAPE_pushed"]);
//         tAnb.addFrame(mb1, 1);
//         //shared_ptr<UI_button> elementb1("main_menu_button", mb1, &tAnb);
//         //main_ui_elements.push_back(elementb1);
//     }

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    // load textures finished

    // initialize main timer (to measure fps, schedule events, etc.)
    Clock frameClock;

    std::shared_ptr<SceneController> scene_controller = std::make_shared<SceneController>(window, resload);
    scene_controller->load_config();
    loading_logger->info("SceneController loading scenes complete");

    // Field editor scene. At first it is inactive.
    shared_ptr<Scene_editor> editor_scene = std::dynamic_pointer_cast<Scene_editor>(scene_controller->get_scene("editor_scene"));

    // current ready animations
    // each animation are loaded x4, because no resourceloader
    vector<string> player_animation_fnames =
    {
        "Images/Flametail/idle_animation_0.png",
        "Images/Flametail/idle_animation_2.png",
        "Images/Flametail/movement_0.png",
        "Images/Flametail/movement_2.png"
    };

    // Create first and only player.
    // Player knows, where he stands, and the field also
    editor_scene->get_field(0)->addPlayer(player_animation_fnames);
    shared_ptr<Player> player_0 = editor_scene->get_field(0)->player_0;
    editor_scene->get_field(1)->player_0 = player_0;

    editor_scene->set_bound_callbacks(sf::Keyboard::Tab, tom_and_jerry(*editor_scene));

    loading_logger->info("Loaded fields");

/////////////////////////////////////////////////////////////////////////////////////////////////////////
///----------------------------------------= START programm =--------------------------------------------
/////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Well, because we start with main menu
    scene_controller->set_current_scene("main_menu");

    loading_logger->info("Starting main loop");

    // main loop
    while (window->isOpen())
    {
        // Handle events loop
        Event event;
        std::shared_ptr<Scene> cur_scene = scene_controller->get_current_scene();

        while (window->pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(*window, event);

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

        ImGui::SFML::Update(*window, frameTime);

        // ImGui::ShowDemoWindow();

        if (cur_scene)
            cur_scene->update(frameTime);


        // clear previous frame and draw from scratch
        window->clear();
        if (cur_scene)
        {
            cur_scene->draw_buffers();
            window->draw(*cur_scene);
        }

        ImGui::SFML::Render(*window);

        window->display();

    }

    return 0;
}
