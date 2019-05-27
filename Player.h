#include <SFML/System/Vector2.hpp>
#include <string>

#include "Inventory.h"

using namespace std;
using namespace sf;

class Player
{
private:
    string name;
    int type;
    Inventory backpack;
    Vector2i coords;
    string appearence;

public:
    Player();
    Player(string name, int type, int backpack_size, Vector2i coords);
};
