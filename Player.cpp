#include "Player.h"

Player::Player()
{
    name = "Player";
    type = 0;
    backpack = Inventory());
    coords = Vector2i(0, 0);
    appearence = "player";
}

Player::Player(string name, int type, int backpack_size, Vector2i coords)
{
    name = name;
    type = type;
    backpack = Inventory(backpack_size);
    coords = coords;
}
