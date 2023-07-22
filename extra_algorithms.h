#ifndef EXTRA_ALGORITHMS
#define EXTRA_ALGORITHMS

#include <iostream>
#include <string>
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

class Scene;

// extract filename (without extension) from full path
std::string re_name(std::string path);

// get direction from vector2f movement; Direction is enumerate 0, 1, 2, 3: east, south, west, north
int direction_from_shift(sf::Vector2f shift);

bool comparator_pair_time_func(std::pair<sf::Time, std::function<void()> > a,
                               std::pair<sf::Time, std::function<void()> > b);

// creates callback to change scene in SceneController
std::function<void()> create_change_scene_callback(std::shared_ptr<Scene> scene, std::string scene_to);
// creates callback that closes window
std::function<void()> create_window_closed_callback(std::shared_ptr<sf::RenderWindow> window);
/// MEMORY LEAK
// creates semi-transparent colored sprite, that covers field
std::function<void()> create_bloodscreen(std::shared_ptr<Scene> scene, const sf::Color& color = sf::Color(250, 0, 0, 50));
// cleares scene->sprites
std::function<void()> clear_bloodscreen(std::shared_ptr<Scene> scene);

void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect, sf::IntRect texFrame);
void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect);

extern int direction_x[4];
extern int direction_y[4];

#endif
