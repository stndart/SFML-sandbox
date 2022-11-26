#include "extra_algorithms.h"

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

int direction_from_shift(sf::Vector2f shift)
{
    int direction;
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
            direction = 3;
        else
            direction = 1;
    }
    return direction;
}
