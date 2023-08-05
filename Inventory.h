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

// Inventory stores all kinds of Items and is associated with Character.
// It provides storage, limitations of storage, and add-remove methods
class Inventory
{
private:
    // Storage limits. -1 means no limit.
    int max_size = -1;
    float max_weight = -1;
    float max_volume = -1;

    // current size, etc. of storage
    int current_size = 0;
    float current_weight = 0;
    float current_volume = 0;

    // flag is set whenever storage has changes. Used by InventoryDraw to maintain all Items displayed
    bool inventory_changed = true;

    // sotrage sorted by categories and names;
    std::map<std::string, std::map<std::string, std::vector<Item> > > storage;

    // associated Character. Each Character has only one Inventory
    Character* owner;

    std::shared_ptr<spdlog::logger> map_events_logger;

public:
    Inventory(Character* owner);
    Inventory(Character* owner, int n_max_size, float n_max_weight, float n_max_volume);

    int get_max_size();
    void set_inventory_changed(bool changed);
    bool is_inventory_changed() const;

    // adds one item to appopriate category
    void addItem(Item thing);
    // asks, how many items of name <name> storage has
    int askItem(std::string name, int min_quality=0);
    // gets list of items by name
    std::vector<Item> getItem(std::string name, int min_quality=0);
    // gets and removes from storage list of items by name
    std::vector<Item> takeItem(std::string name, int min_quality=0);

    // same for category. Item with any category passes, as long it has same category
    int askCategory(std::string category, int min_quality=0);
    std::vector<Item> getCategory(std::string category, int min_quality=0);
    std::vector<Item> takeCategory(std::string category, int min_quality=0);
};

#endif // INVENTORY_H