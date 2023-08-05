#ifndef ITEM_H
#define ITEM_H

#include <string>

// Item is an item. It has its own weight, volume, quality, but shares same name and category with some other Items.
// Items with same names are considered the same
class Item
{
private:
    float weight;
    float volume;

    float quality = 1;

public:
    std::string name;
    std::string category;

    Item(float weight, float volume = 1, float quality = 1);
    Item(std::string name, float weight = 1, float volume = 1, float quality = 1);
    Item(std::string name, std::string category, float weight = 1, float volume = 1, float quality = 1);

    float getWeight() const;
    float getVolume() const;
    float getQuality() const;
};

#endif // ITEM_H