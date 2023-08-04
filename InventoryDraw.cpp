#include "InventoryDraw.h"

InventoryDraw::InventoryDraw(
    std::string name, sf::IntRect UIFrame,
    sf::Vector2f n_element_size, sf::Vector2i n_element_count,
    Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed) :
    UI_window(name, UIFrame, parent, resload, is_framed)
{
    element_size = n_element_size;
    element_count = n_element_count;
    resource_manager = resload;
}

InventoryDraw::InventoryDraw(
    std::string name, sf::IntRect UIFrame, Inventory* bag,
    Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed) :
    UI_window(name, UIFrame, parent, resload, is_framed)
{
    /// MAGICAL NUMBERS
    element_size = sf::Vector2f(80, 80);
    element_count = sf::Vector2i(UIFrame.width / element_size.x, UIFrame.height / element_size.y);
    resource_manager = resload;
    
    linkInventory(bag);
}

std::shared_ptr<InventoryDraw> InventoryDraw::fromInventorySize(
    std::string name, sf::Vector2f n_element_size, sf::Vector2i n_element_count,
    Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed)
{
    sf::IntRect UIFrame(Vector2i(0, 0), Vector2i(n_element_size.x * n_element_count.x, n_element_size.y * n_element_count.y));

    std::shared_ptr<InventoryDraw> idraw = std::make_shared<InventoryDraw>(name, UIFrame, n_element_size, n_element_count, parent, resload, is_framed);
    return idraw;
}

std::shared_ptr<InventoryDraw> InventoryDraw::fromInventory(
    std::string name, Inventory* bag,
    Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed)
{
    /// MAGICAL NUMBERS
    sf::Vector2f n_element_size(80, 80);
    sf::Vector2i n_element_count(5, bag->get_max_size() / 5 + 1);
    sf::IntRect UIFrame(Vector2i(0, 0), Vector2i(n_element_size.x * n_element_count.x, n_element_size.y * n_element_count.y));

    std::shared_ptr<InventoryDraw> idraw = std::make_shared<InventoryDraw>(name, UIFrame, n_element_size, n_element_count, parent, resload, is_framed);
    idraw->linkInventory(bag);
    return idraw;
}

// push item ui element into list
void InventoryDraw::addItemElement(std::shared_ptr<UI_element> new_element, int z_index)
{
    displayed = true;

    new_element->z_index = z_index;
    item_elements.insert(std::make_pair(z_index, new_element));
}

// delete item ui elemen by pointer
void InventoryDraw::deleteItemElement(std::shared_ptr<UI_element> element)
{
    item_elements.erase(std::make_pair(element->z_index, element));
}

void InventoryDraw::view_inventory()
{
    item_elements.clear();

    int row = 0, column = 0;
    for (auto item : bag->getCategory(""))
    {
        sf::Vector2i pos(element_size.x * column, element_size.y * row);

        std::shared_ptr<UI_button> inv_item = std::make_shared<UI_button>(
            "Item: " + item.name, sf::IntRect(pos, sf::Vector2i(element_size)),
            parent_scene, resource_manager->getItemTexture(item.name));
        addItemElement(inv_item, 1);

        column++;
        if (column >= element_count.x)
        {
            column = 0;
            row++;
        }
    }
}

void InventoryDraw::linkInventory(Inventory* new_bag)
{
    bag = new_bag;
    if (bag)
        view_inventory();
}

// override to include item_elements
void InventoryDraw::draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap) const
{
    for (auto& [z_index, element] : item_elements)
        element->draw_to_zmap(zmap);
    
    UI_window::draw_to_zmap(zmap);
}

// update and redraw inventory if needed
void InventoryDraw::update(Time deltaTime)
{
    if (bag->is_inventory_changed())
    {
        view_inventory();
    }
}