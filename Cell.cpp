#include "Cell.h"

Cell::Cell()
{
    type = 0;
    coords = Vector2i(0, 0);
    objects = vector<Object>(0);
    items = vector<Item>(0);
}

Cell::Cell(int t, Vector2i c)
{
    type = t;
    coords = c;
    objects = vector<Object>(0);
    items = vector<Item>(0);
}

vector<Item> Cell::searchfor()
{
    vector<Item> result(0);
    return result;
}
