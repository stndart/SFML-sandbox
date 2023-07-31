# ifndef CALLBACKS_H
# define CALLBACKS_H

#pragma once

#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

class Scene;
class Scene_Field;

// Callbacks.h contains factory for creating conditional events. Usually they are to bound with pressing keys or UI buttons
// Simple factories just return callback which, when called, perform certain action.
// Actions list:
// 1. change current Scene
// 2. change current Field
// 3. close Window
// 4. create visual effect of screen fading to black (or specified color)
// 5. reverse effect to fading
// 6. create colored rectangle
// 7. create colored circle
// 8. create colored circle, but with center above Player
// 9. remove all sprites from either framebuffer in Scene
// 10. block player controls (and most UI buttons)
// Complicated factories return a sequence of callbacks, with delays specified. These factories are used as ready presets to some cool effects, such as:
// 1. Tom and Jerry field transition effect: in the 1st phase darkness gathers aroung player in circle, after which circle fades, then, after field change, actions are repeated in inverse order

// creates callback to change scene in SceneController
std::function<void()> create_change_scene_callback(Scene& scene, std::string scene_to);
// creates callback that closes window
std::function<void()> create_window_closed_callback(sf::RenderWindow& window);
// creates callback that changes fields in scene
std::function<void()> create_change_field_callback(Scene_Field& scene, int field_to=-1);

/// MEMORY LEAK
// creates fogging, that thickens over 2 seconds (color is by default black)
std::function<void()> create_fade_effect(Scene& scene, const sf::Color& color = sf::Color(0, 0, 0, 50),
                                         sf::Time duration = sf::seconds(2), int z_ind = 0, sf::BlendMode blend_mode = sf::BlendNone);
/// MEMORY LEAK
// creates fogging, that fades over 2 seconds (color is by default black)
std::function<void()> create_rfade_effect(Scene& scene, const sf::Color& color = sf::Color(0, 0, 0, 50),
                                          sf::Time duration = sf::seconds(2), int z_ind = 0, sf::BlendMode blend_mode = sf::BlendNone);
/// MEMORY LEAK
// creates semi-transparent circle, that covers field
std::function<void()> create_rect(Scene& scene, sf::FloatRect posrect, const sf::Color& color = sf::Color(100, 100, 0, 150));
/// MEMORY LEAK
// creates semi-transparent circle, that covers field
std::function<void()> create_light_circle(Scene& scene, sf::Vector2f pos = sf::Vector2f(960, 540),
                                          float radius = 70, const sf::Color& color = sf::Color(100, 100, 0, 150));
/// MEMORY LEAK
// creates semi-transparent circle, around player, with animated radius
std::function<void()> create_light_circle_centered(Scene_Field& scene,
                                                   float radius_1 = 500, float radius_2 = 30, const sf::Color& color = sf::Color(200, 200, 100, 150),
                                                   sf::Time duration = sf::seconds(0.4));
// cleares scene->sprites
std::function<void()> clear_scene_sprites(Scene& scene);

// blocks or unblocks WASD controls
std::function<void()> create_block_controls_callback(Scene_Field& scene, bool block);

// Tom and Jerry fade effect (narrowing circle, then fade)
std::vector<std::pair<std::function<void()>, sf::Time> > tom_and_jerry_fade_effect(Scene_Field& scene, float min_radius,
                                                sf::Time circle_duration = sf::seconds(0.4), sf::Time fade_duration = sf::seconds(0.2));
// Tom and Jerry rfade effect (rfade, then broadening circle
std::vector<std::pair<std::function<void()>, sf::Time> > tom_and_jerry_rfade_effect(Scene_Field& scene, float min_radius,
                                                sf::Time offset = sf::seconds(0.6), sf::Time circle_duration = sf::seconds(0.4), sf::Time fade_duration = sf::seconds(0.2));
// Tom and Jerry united effect
std::vector<std::pair<std::function<void()>, sf::Time> > tom_and_jerry(Scene_Field& scene, float min_radius=50,
                                                                       sf::Time circle_duration = sf::seconds(0.4), sf::Time fade_duration = sf::seconds(0.2));

# endif // CALLBACKS_H
