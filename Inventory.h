#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include "Item.h"

#include <spdlog/spdlog.h>

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

    bool inventory_changed = true;

    std::map<std::string, std::map<std::string, std::vector<Item> > > storage;

    Character* owner;

    std::shared_ptr<spdlog::logger> map_events_logger;

public:
    Inventory(Character* owner);
    Inventory(Character* owner, int n_max_size, float n_max_weight, float n_max_volume);

    int get_max_size();
    void set_inventory_changed(bool changed);
    bool is_inventory_changed() const;

    void addItem(Item thing);
    int askItem(std::string name, int min_quality=0);
    std::vector<Item> getItem(std::string name, int min_quality=0);
    std::vector<Item> takeItem(std::string name, int min_quality=0);

    int askCategory(std::string category, int min_quality=0);
    std::vector<Item> getCategory(std::string category, int min_quality=0);
    std::vector<Item> takeCategory(std::string category, int min_quality=0);
};

#endif // INVENTORY_H