#include "Item.h"

Item::Item(std::string name, float weight, float volume, float quality) : weight(weight), volume(volume), quality(quality), name(name), category("")
{
    // None
}

Item::Item(float weight, float volume, float quality) : Item("", weight, volume, quality)
{
    // None
}

Item::Item(std::string name, std::string n_category, float weight, float volume, float quality) : Item(name, weight, volume, quality)
{
    category = n_category;
}

float Item::getWeight() const
{
    return weight;
}

float Item::getVolume() const
{
    return volume;
}

float Item::getQuality() const
{
    return quality;
}