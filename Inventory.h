#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "Item.h"

class Character;

class Inventory
{
private:
    int max_size = -1;
    float max_weight = -1;
    float max_volume = -1;

    int current_size = 0;
    float current_weight = 0;
    float current_volume = 0;

    std::map<std::string, std::map<std::string, std::vector<Item> > > storage;

    Character* owner;

public:
    Inventory();
    Inventory(int max_size, float max_weight, float max_volume);

    void addItem(Item thing);
    int askItem(std::string name, int min_quality=0);
    std::vector<Item> takeItem(std::string name, int min_quality=0);

    int askCategory(std::string category, int min_quality=0);
    std::vector<Item> takeCategory(std::string category, int min_quality=0);
};

#endif // INVENTORY_H