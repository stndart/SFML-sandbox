#ifndef INVENTORYDRAW_H
#define INVENTORYDRAW_H

#include <memory>

#include "UI_element.h"
#include "UI_window.h"
#include "ResourceLoader.h"
#include "Inventory.h"

class InventoryDraw : public UI_window
{
private:
    Inventory* bag;

    sf::Vector2f element_size;
    sf::Vector2i element_count;

    bool inventory_changed = false;

    std::shared_ptr<ResourceLoader> resource_manager;

public:
    static std::shared_ptr<InventoryDraw> fromInventorySize(
        std::string name, sf::Vector2f element_size, sf::Vector2i element_count,
        Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);
    static std::shared_ptr<InventoryDraw> fromInventory(
        std::string name, Inventory* bag,
        Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);

    InventoryDraw(
        std::string name, sf::IntRect UIFrame, sf::Vector2f element_size, sf::Vector2i element_count,
        Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);
    
    void set_inventory_changed(bool changed);

    void view_inventory();
    void linkInventory(Inventory* bag);

    // update and redraw inventory if needed
    void update(Time deltaTime) override;
};

#endif // INVENTORYDRAW_H