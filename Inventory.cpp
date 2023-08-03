#include "Inventory.h"

Inventory::Inventory()
{
    // None
}

Inventory::Inventory(int max_size, float max_weight, float max_volume) : max_size(max_size), max_weight(max_weight), max_volume(max_volume)
{
    // None
}


void Inventory::addItem(Item thing)
{
    storage[thing.category][thing.name].push_back(thing);
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

std::vector<Item> Inventory::takeItem(std::string name, int min_quality)
{
    std::vector<Item> giveaway;
    for (auto& [cat_name, categories] : storage)
        for (auto& [tname, item_list] : categories)
            for (std::vector<Item>::iterator thing = item_list.begin(); thing != item_list.end(); thing++)
                if (thing->name == name && thing->getQuality() >= min_quality)
                {
                    giveaway.push_back(*thing);
                    std::iter_swap(thing, item_list.end() - 1);
                    item_list.pop_back();
                }
    
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

std::vector<Item> Inventory::takeCategory(std::string category, int min_quality)
{
    std::vector<Item> giveaway;
    for (auto& [cat_name, categories] : storage)
        for (auto& [tname, item_list] : categories)
            for (std::vector<Item>::iterator thing = item_list.begin(); thing != item_list.end(); thing++)
                if (thing->getQuality() >= min_quality)
                {
                    giveaway.push_back(*thing);
                    std::iter_swap(thing, item_list.end() - 1);
                    item_list.pop_back();
                }
    
    return giveaway;
}
