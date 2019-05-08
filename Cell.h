#include <SFML/System/Vector2.hpp>
#include <vector>

#include "Object.h"
#include "Item.h"

using namespace std;
using namespace sf;


class Cell
{
public:
    int type;
    Vector2i coords;
    vector<Object> objects;
    vector<Item> items;

    vector<Item> searchfor();

    Cell();
    Cell(int t, Vector2i c);
};
