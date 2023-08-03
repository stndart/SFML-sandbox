#ifndef ITEM_H
#define ITEM_H

#include <string>

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