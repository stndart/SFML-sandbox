#include "extra_algorithms.h"

// extract filename (without extension) from full path
std::string re_name(std::string path)
{
    std::string result = "";
    for (unsigned int i = 0; i < path.size(); i++)
    {
        if (path[i] == '/')
        {
            result = "";
            continue;
        }
        else if (path[i] == '.')
        {
            break;
        }
        else result += path[i];
    }
    return result;
}

// get direction from vector2f movement; Direction is enumerate 0, 1, 2, 3: east, south, west, north
int direction_from_shift(sf::Vector2f shift)
{
    if (shift == sf::Vector2f(0, 0))
        return -1; // default undirected direction

    int direction = 0;
    if (abs(shift.x) > abs(shift.y))
    {
        if (shift.x > 0)
            direction = 0;
        else
            direction = 2;
    }
    else
    {
        if (shift.y > 0)
            direction = 1;
        else
            direction = 3;
    }
    return direction;
}

int direction_x[4] = {1, 0, -1, 0};
int direction_y[4] = {0, 1, 0, -1};

