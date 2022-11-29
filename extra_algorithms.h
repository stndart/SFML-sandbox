#ifndef EXTRA_ALGORITHMS
#define EXTRA_ALGORITHMS

#include <SFML/Graphics.hpp>
#include <iostream>

std::string re_name(std::string path);

int direction_from_shift(sf::Vector2f shift);

extern int direction_x[4];
extern int direction_y[4];

#endif
