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

        loading_logger->set_level(spdlog::level::trace);
        input_logger->set_level(spdlog::level::info);
        map_events_logger->set_level(spdlog::level::debug);
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

    // resource manager (lazy_ram, lazy_gpu)
    std::shared_ptr<ResourceLoader> resload = std::make_shared<ResourceLoader>(true, true);

    loading_logger->info("Resource manager texture load complete");

    // initialize main timer (to measure fps, schedule events, etc.)
    Clock frameClock;

    std::shared_ptr<SceneController> scene_controller = std::make_shared<SceneController>(window, resload);
    scene_controller->load_config();
    loading_logger->info("SceneController loading scenes complete");

    // Field editor scene. At first it is inactive.
    shared_ptr<Scene_editor> editor_scene = std::dynamic_pointer_cast<Scene_editor>(scene_controller->get_scene("editor_scene"));

    // Create first and only player.
    std::shared_ptr<Player> player = std::make_shared<Player>("Player_0");
    std::unique_ptr<Character> Flametail = std::make_unique<Character>("Flametail", resload);
    Flametail->load_config("configs/characters/flametail.json");
    player->setCharacter(std::move(Flametail));

    editor_scene->get_field(0)->player_0 = player;
    editor_scene->get_field(1)->player_0 = player;
    editor_scene->get_field(0)->teleport_to();

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

    loading_logger->info("Main cycle finished, shutting down");

    return 0;
}
