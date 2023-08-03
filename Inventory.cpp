#include "Inventory.h"

#include "Character.h"

Inventory::Inventory(Character* owner) : owner(owner)
{
    // Reaching out to global "map_events" logger by name
    map_events_logger = spdlog::get("map_events");
}

Inventory::Inventory(Character* owner, int n_max_size, float n_max_weight, float n_max_volume) : Inventory(owner)
{
    max_size = n_max_size;
    max_weight = n_max_weight;
    max_volume = n_max_volume;
}

int Inventory::get_max_size()
{
    return max_size;
}

void Inventory::set_inventory_changed(bool changed)
{
    inventory_changed = changed;
}

bool Inventory::is_inventory_changed() const    
{
    return inventory_changed;
}

void Inventory::addItem(Item thing)
{
    if (
        current_size + 1 > max_size ||
        current_volume + thing.getVolume() > max_volume ||
        current_weight + thing.getWeight() > max_weight
    )
    {
        map_events_logger->warn("Trying to fit unfittable {} into {}'s bag", thing.name, owner->name);
        return;
    }

    storage[thing.category][thing.name].push_back(thing);

    current_size += 1;
    current_volume += thing.getVolume();
    current_weight += thing.getWeight();

    set_inventory_changed(true);
}

int Inventory::askItem(std::string name, int min_quality)
{
    int count = 0;
    for (auto& [cat_name, categories] : storage)
        for (auto& [tname, item_list] : categories)
            for (auto thing : item_list)
                if (tname == name && thing.getQuality() >= min_quality)
                    count++;
    
    return count;
}

std::vector<Item> Inventory::getItem(std::string name, int min_quality)
{
    std::vector<Item> giveaway;
    for (auto& [cat_name, categories] : storage)
        for (auto& [tname, item_list] : categories)
            for (std::vector<Item>::iterator thing = item_list.begin(); thing != item_list.end(); thing++)
                if (thing->name == name && thing->getQuality() >= min_quality)
                    giveaway.push_back(*thing);
    
    return giveaway;
}

std::vector<Item> Inventory::takeItem(std::string name, int min_quality)
{
    std::vector<Item> giveaway;
    for (auto& [cat_name, categories] : storage)
        for (auto& [tname, item_list] : categories)
            for (std::vector<Item>::iterator thing = item_list.begin(); thing != item_list.end(); thing++)
                if (thing->name == name && thing->getQuality() >= min_quality)
                {
                    current_size -= 1;
                    current_volume -= thing->getVolume();
                    current_weight -= thing->getWeight();

                    giveaway.push_back(*thing);
                    std::iter_swap(thing, item_list.end() - 1);
                    item_list.pop_back();

                }
    
    set_inventory_changed(true);
    return giveaway;
}


int Inventory::askCategory(std::string category, int min_quality)
{
    int count = 0;
    for (auto& [cat_name, categories] : storage)
        if (cat_name == category)
            for (auto& [tname, item_list] : categories)
                for (auto thing : item_list)
                    if (thing.getQuality() >= min_quality)
                        count++;
    
    return count;
}

std::vector<Item> Inventory::getCategory(std::string category, int min_quality)
{
    std::vector<Item> giveaway;
    for (auto& [cat_name, categories] : storage)
        for (auto& [tname, item_list] : categories)
            for (std::vector<Item>::iterator thing = item_list.begin(); thing != item_list.end(); thing++)
                if (thing->getQuality() >= min_quality)
                    giveaway.push_back(*thing);
    
    return giveaway;
}

std::vector<Item> Inventory::takeCategory(std::string category, int min_quality)
{
    std::vector<Item> giveaway;
    for (auto& [cat_name, categories] : storage)
        for (auto& [tname, item_list] : categories)
            for (std::vector<Item>::iterator thing = item_list.begin(); thing != item_list.end(); thing++)
                if (thing->getQuality() >= min_quality)
                {
                    current_size -= 1;
                    current_volume -= thing->getVolume();
                    current_weight -= thing->getWeight();

                    giveaway.push_back(*thing);
                    std::iter_swap(thing, item_list.end() - 1);
                    item_list.pop_back();
                }
    
    set_inventory_changed(true);
    return giveaway;
}
