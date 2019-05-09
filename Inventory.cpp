#include "Inventory.h"

Inventory::Inventory()
{
    size = 0;
    items = vector<Item>(0);
}

Inventory::Inventory(int s)
{
    size = s;
    items = vector<Item>(s);
}
