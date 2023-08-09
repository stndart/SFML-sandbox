
#include "Callbacks.h"

#include "Scene.h"
#include "Scene_Field.h"
#include "SceneController.h"

// creates callback to change scene in SceneController
std::function<void()> create_change_scene_callback(Scene& scene, std::string scene_to)
{
    std::function<void()> callback = [&scene, scene_to]{
        scene.get_scene_controller().set_current_scene(scene_to);
    };
    return callback;
}

// creates callback that closes window
std::function<void()> create_window_closed_callback(sf::RenderWindow& window)
{
    std::function<void()> callback = [&window] {
        window.close();
    };
    return callback;
}

// creates callback that changes fields in scene
std::function<void()> create_change_field_callback(Scene_Field& scene, int field_to)
{
    std::function<void()> callback = [&scene, field_to]{
        scene.change_current_field(field_to);
    };
    return callback;
}


// closes UI_window by name
std::function<void()> close_UI_window_callback(Scene& scene, std::string UI_window_name)
{
    std::function<void()> callback = [&scene, UI_window_name] {
        scene.show_UI_window(UI_window_name, false);
    };
    return callback;
}

// opens or creates UI_window by name
std::function<void()> open_UI_window_callback(Scene& scene, std::string UI_window_name)
{
    std::function<void()> callback = [&scene, UI_window_name] {
        std::shared_ptr<UI_window> newwindow = scene.create_subwindow(UI_window_name);
    };
    return callback;
}

// creates fogging, that thickens through 2 seconds (color is by default black)
std::function<void()> create_fade_effect(Scene& scene, const sf::Color& color, sf::Time duration, int z_ind, sf::BlendMode blend_mode)
{
    sf::Color color_transparent = sf::Color(color.r, color.g, color.b, 0);
    std::function<void()> callback = [&scene, duration, color, color_transparent, z_ind, blend_mode] {
        std::shared_ptr<AnimatedSprite> as = std::make_shared<AnimatedSprite>("fade effect",
                                                std::make_unique<RectangleShape>(Vector2f(1920, 1080)), Vector2f(0, 0), Vector2f(0, 0), z_ind, blend_mode);
        State start = {0, color_transparent,
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        State finish = {0, color,
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        std::shared_ptr<VisualEffect> ve = std::make_shared<VisualEffect>(std::move(as), seconds(0), duration, start, finish);
        ve->play();
        scene.addSprite(std::move(ve), z_ind, true);
    };
    return callback;
}

// creates fogging, that fades through 2 seconds (color is by default black)
std::function<void()> create_rfade_effect(Scene& scene, const sf::Color& color, sf::Time duration, int z_ind, sf::BlendMode blend_mode)
{
    sf::Color color_transparent = sf::Color(color.r, color.g, color.b, 0);
    std::function<void()> callback = [&scene, duration, color, color_transparent, z_ind, blend_mode] {
        std::shared_ptr<AnimatedSprite> as = std::make_shared<AnimatedSprite>("reverse fade effect",
                                                std::make_unique<RectangleShape>(Vector2f(1920, 1080)), Vector2f(0, 0), Vector2f(0, 0), z_ind, blend_mode);
        State start = {0, color,
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        State finish = {0, color_transparent,
            Vector2f(0, 0), Vector2f(0, 0), Vector2f(1, 1)};
        std::shared_ptr<VisualEffect> ve = std::make_shared<VisualEffect>(std::move(as), seconds(0), duration, start, finish);
        ve->play();
        scene.addSprite(std::move(ve), z_ind, true);
    };
    return callback;
}

// creates semi-transparent circle, that covers field
std::function<void()> create_rect(Scene& scene, sf::FloatRect posrect, const sf::Color& color)
{
    std::function<void()> callback = [&scene, posrect, color] {
        std::unique_ptr<RectangleShape> rect = std::make_unique<RectangleShape>(posrect.getSize());
        rect->setFillColor(color);
        std::shared_ptr<AnimatedSprite> as = std::make_shared<AnimatedSprite>("dark rect",
                                                std::move(rect), posrect.getPosition(), Vector2f(0, 0), 0, sf::BlendNone);
        scene.addSprite(std::move(as), 0, true);
    };
    return callback;
}

// creates semi-transparent circle, that covers field
std::function<void()> create_light_circle(Scene& scene, sf::Vector2f pos, float radius, const sf::Color& color)
{
    std::function<void()> callback = [&scene, pos, radius, color] {
        std::unique_ptr<CircleShape> circle = std::make_unique<CircleShape>(radius, 100);
        circle->setFillColor(color);
        std::shared_ptr<AnimatedSprite> as = std::make_shared<AnimatedSprite>(
            "light circle", std::move(circle), pos, Vector2f(0, 0), 0, sf::BlendNone);
        scene.addSprite(std::move(as), 1, true);
    };
    return callback;
}

// creates semi-transparent circle, around player, with animated radius
std::function<void()> create_light_circle_centered(Scene_Field& scene,
                                                   float radius_1, float radius_2, const sf::Color& color, sf::Time duration)
{
    std::function<void()> callback = [&scene, radius_1, radius_2, color, duration] {
        std::unique_ptr<CircleShape> circle = std::make_unique<CircleShape>(radius_1, 100);
        circle->setFillColor(color);
        sf::FloatRect player_bounds = scene.getPlayerGlobalBounds();
        sf::Vector2f player_pos = player_bounds.getPosition() + player_bounds.getSize() / 2.0f;

        std::shared_ptr<AnimatedSprite> as = std::make_shared<AnimatedSprite>("light circle",
                                                std::move(circle), player_pos, Vector2f(radius_1, radius_1), 0, sf::BlendNone);
        std::shared_ptr<VisualEffect> ve = std::make_shared<VisualEffect>(std::move(as), sf::seconds(0), duration,
                                            State{0, color, player_pos, Vector2f(0, 0), Vector2f(1, 1)},
                                            State(0, color, player_pos, Vector2f(0, 0), Vector2f(radius_2 / radius_1, radius_2 / radius_1)),
                                            1, sf::BlendNone);
        ve->play();
        scene.addSprite(std::move(ve), 1, true);
    };
    return callback;
}

// cleares scene->sprites
std::function<void()> clear_scene_sprites(Scene& scene)
{
    std::function<void()> callback = [&scene] {
        scene.delete_sprites();
    };
    return callback;
}

// blocks or unblocks WASD controls
std::function<void()> create_block_controls_callback(Scene_Field& scene, bool block)
{
    std::function<void()> callback = [&scene, block] {
        scene.block_controls(block);
    };
    return callback;
}

// Tom and Jerry fade effect (narrowing circle, then fade)
std::vector<std::pair<std::function<void()>, sf::Time> > tom_and_jerry_fade_effect(Scene_Field& scene, float min_radius,
                                                                                   sf::Time circle_duration, sf::Time fade_duration)
{
    std::vector<std::pair<std::function<void()>, sf::Time> > sequence;
    sequence.push_back(std::make_pair(create_block_controls_callback(scene, true), sf::seconds(0)));

    sequence.push_back(std::make_pair(create_rect(scene, sf::FloatRect(0, 0, 1920, 1080), sf::Color(0, 0, 0, 255)), sf::seconds(0)));
    sequence.push_back(std::make_pair(create_light_circle_centered(scene, 1500, 70, sf::Color(0, 0, 0, 0), circle_duration), sf::seconds(0)));
    sequence.push_back(std::make_pair(create_fade_effect(scene, sf::Color(0, 0, 0, 255), fade_duration, 2, sf::BlendAlpha), circle_duration));

    sequence.push_back(std::make_pair(clear_scene_sprites(scene), circle_duration + fade_duration));
    sequence.push_back(std::make_pair(create_block_controls_callback(scene, false), circle_duration + fade_duration));

    return sequence;
}

// Tom and Jerry rfade effect (rfade, then broadening circle
std::vector<std::pair<std::function<void()>, sf::Time> > tom_and_jerry_rfade_effect(Scene_Field& scene, float min_radius,
                                                                                    sf::Time offset, sf::Time circle_duration, sf::Time fade_duration)
{
    std::vector<std::pair<std::function<void()>, sf::Time> > sequence;
    sequence.push_back(std::make_pair(create_block_controls_callback(scene, true), offset));

    sequence.push_back(std::make_pair(create_rect(static_cast<Scene&>(scene), sf::FloatRect(0, 0, 1920, 1080), sf::Color(0, 0, 0, 255)), offset));
    sequence.push_back(std::make_pair(create_light_circle_centered(scene, 70, 70, sf::Color(0, 0, 0, 0), fade_duration), offset));
    sequence.push_back(std::make_pair(create_light_circle_centered(scene, 70, 1500, sf::Color(0, 0, 0, 0), circle_duration), offset + fade_duration));
    sequence.push_back(std::make_pair(create_rfade_effect(static_cast<Scene&>(scene), sf::Color(0, 0, 0, 255), fade_duration, 2, sf::BlendAlpha), offset));

    sequence.push_back(std::make_pair(clear_scene_sprites(static_cast<Scene&>(scene)), offset + fade_duration + circle_duration));
    sequence.push_back(std::make_pair(create_block_controls_callback(scene, false), offset + fade_duration + circle_duration));
    return sequence;
}

// Tom and Jerry united effect
std::vector<std::pair<std::function<void()>, sf::Time> > tom_and_jerry(Scene_Field& scene, float min_radius,
                                                                       sf::Time circle_duration, sf::Time fade_duration)
{
    auto taj_fade = tom_and_jerry_fade_effect(scene, min_radius, circle_duration, fade_duration);
    auto taj_rfade = tom_and_jerry_rfade_effect(scene, min_radius, circle_duration + fade_duration, circle_duration, fade_duration);

    taj_fade.insert(taj_fade.end(), std::make_pair(create_change_field_callback(scene), circle_duration + fade_duration));
    taj_fade.insert(taj_fade.end(), taj_rfade.begin(), taj_rfade.end());
    return taj_fade;
}
