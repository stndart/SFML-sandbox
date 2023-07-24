#ifndef EXTRA_ALGORITHMS
#define EXTRA_ALGORITHMS

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <filesystem>

#include <SFML/Graphics.hpp>

class Scene;
class Scene_Field;

// extract filename (without extension) from full path
std::string re_name(std::string path);

// get direction from vector2f movement; Direction is enumerate 0, 1, 2, 3: east, south, west, north
int direction_from_shift(sf::Vector2f shift);

// creates callback to change scene in SceneController
std::function<void()> create_change_scene_callback(std::shared_ptr<Scene> scene, std::string scene_to);
// creates callback that closes window
std::function<void()> create_window_closed_callback(std::shared_ptr<sf::RenderWindow> window);
// creates callback that changes fields in scene
std::function<void()> create_change_field_callback(std::shared_ptr<Scene_Field> scene, int field_to=-1);

/// MEMORY LEAK
// creates fogging, that thickens over 2 seconds (color is by default black)
std::function<void()> create_fade_effect(std::shared_ptr<Scene> scene, const sf::Color& color = sf::Color(0, 0, 0, 50), sf::Time duration = sf::seconds(2));
/// MEMORY LEAK
// creates fogging, that fades over 2 seconds (color is by default black)
std::function<void()> create_rfade_effect(std::shared_ptr<Scene> scene, const sf::Color& color = sf::Color(0, 0, 0, 50), sf::Time duration = sf::seconds(2));
/// MEMORY LEAK
// creates semi-transparent circle, that covers field
std::function<void()> create_light_circle(std::shared_ptr<Scene> scene, sf::Vector2f pos = sf::Vector2f(960, 540), float radius = 70, const sf::Color& color = sf::Color(100, 100, 0, 150));
// cleares scene->sprites
std::function<void()> clear_scene_sprites(std::shared_ptr<Scene> scene);

void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect, sf::IntRect texFrame);
void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect);

extern int direction_x[4];
extern int direction_y[4];

#endif
