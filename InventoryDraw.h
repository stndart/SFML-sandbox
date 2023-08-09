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

    // displayed size of one item
    sf::Vector2f element_size;
    // elements grid dimensions
    sf::Vector2i element_count;

    // list of items ui elements with z-indexes
    std::set<std::pair<int, std::shared_ptr<UI_element> > > item_elements;

    std::shared_ptr<ResourceLoader> resource_manager;

public:
    // constructs an inventory display with given grid size
    static std::shared_ptr<InventoryDraw> fromInventorySize(
        std::string name, sf::Vector2f element_size, sf::Vector2i element_count,
        Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);
    // constructs an inventory display with given inventory
    static std::shared_ptr<InventoryDraw> fromInventory(
        std::string name, Inventory* bag,
        Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);

    InventoryDraw(
        std::string name, sf::IntRect UIFrame, sf::Vector2f element_size, sf::Vector2i element_count,
        Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);
    InventoryDraw(
        std::string name, sf::IntRect UIFrame, Inventory* bag,
        Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);
        
    // sets displayed for self and child elements
    // overriden because needs to process item_elements
    void show(bool displayed = true) override;
    
    // override setFrame, because element_count changes
    void setFrame(sf::IntRect new_frame_scale) override;

    // push item ui element into list
    void addItemElement(std::shared_ptr<UI_element> new_element, int z_index = 0);
    // delete item ui element by pointer
    void deleteItemElement(std::shared_ptr<UI_element> element);

    // resets and adds all items from linked inventory into <item_elements>
    void view_inventory();

    // sets linked inventory to look items from
    void linkInventory(Inventory* bag);

    // overriding Transformable methods to support nested windows
    void setPosition(const Vector2f &position) override;
    void setOrigin(const Vector2f &origin) override;
    void setScale(const Vector2f &factors) override;

    // override to include item_elements
    void draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap, int z_shift = 0) const override;

    // update and redraw inventory if needed
    void update(Time deltaTime) override;
};

#endif // INVENTORYDRAW_H