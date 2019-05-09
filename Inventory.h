#include <vector>

#include "Item.h"

using namespace std;
using namespace sf;

class Inventory
{
private:
    int size;
    vector<Item> items;

public:
    Inventory();
    Inventory(int s);
};
