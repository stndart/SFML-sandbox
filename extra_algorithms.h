#ifndef EXTRA_ALGORITHMS
#define EXTRA_ALGORITHMS

#include <iostream>
#include <string>
#include <memory>

#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

class Scene;
class Scene_Field;

// extract filename (without extension) from full path
std::string re_name(std::string path);

// get direction from vector2f movement; Direction is enumerate 0, 1, 2, 3: east, south, west, north
int direction_from_shift(sf::Vector2f shift);

// gets coordinate-like absolute and relative properties from json and constructs a vector
sf::Vector2f get_coords_from_json(nlohmann::json j, sf::Vector2i windowsize);
sf::Vector2f get_size_from_json(nlohmann::json j, sf::Vector2i windowsize);
sf::Vector2f get_origin_from_json(nlohmann::json j, sf::Vector2f texsize);

sf::Vector2f save_aspect_ratio(sf::Vector2f new_size, sf::Vector2f old_size);

void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect, sf::IntRect texFrame);
void cutout_texture_to_frame(sf::Vertex m_vertices[4], sf::IntRect rect);

extern int direction_x[4];
extern int direction_y[4];

#endif
