#include "Item.h"

Item::Item()
{
    name = string("");
    amount = 0;
}

Item::Item(string n, int a)
{
    name = n;
    amount = a;
}

Item::Item(string n)
{
    name = n;
    amount = 1;
}
