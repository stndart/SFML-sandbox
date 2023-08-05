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

// constructs an inventory display with given grid size
std::shared_ptr<InventoryDraw> InventoryDraw::fromInventorySize(
    std::string name, sf::Vector2f n_element_size, sf::Vector2i n_element_count,
    Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed)
{
    sf::IntRect UIFrame(Vector2i(0, 0), Vector2i(n_element_size.x * n_element_count.x, n_element_size.y * n_element_count.y));

    std::shared_ptr<InventoryDraw> idraw = std::make_shared<InventoryDraw>(name, UIFrame, n_element_size, n_element_count, parent, resload, is_framed);
    return idraw;
}

// constructs an inventory display with given inventory
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

// sets displayed for self and child elements
// overriden because needs to process item_elements
void InventoryDraw::show(bool disp)
{
    UI_window::show(disp);
    for (auto& [z_index, elem] : item_elements)
        elem->displayed = disp;
}

// override setFrame, because element_count changes
void InventoryDraw::setFrame(sf::IntRect new_frame_scale)
{
    UI_window::setFrame(new_frame_scale);

    // update element_count using old element_size
    element_count = sf::Vector2i(Frame_scale.width / element_size.x, Frame_scale.height / element_size.y);
}

// push item ui element into list
void InventoryDraw::addItemElement(std::shared_ptr<UI_element> new_element, int z_index)
{
    displayed = true;

    new_element->z_index = z_index;
    new_element->set_parent_coords(sf::Vector2f(Frame_scale.getPosition()));
    item_elements.insert(std::make_pair(z_index, new_element));
}

// delete item ui element by pointer
void InventoryDraw::deleteItemElement(std::shared_ptr<UI_element> element)
{
    item_elements.erase(std::make_pair(element->z_index, element));
}

// resets and adds all items from linked inventory into <item_elements>
void InventoryDraw::view_inventory()
{
    item_elements.clear();

    int row = 0, column = 0;
    // position items on grid
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

// sets linked inventory to look items from
void InventoryDraw::linkInventory(Inventory* new_bag)
{
    bag = new_bag;
    if (bag)
        view_inventory();
}

// overriding Transformable methods to support nested windows
void InventoryDraw::setPosition(const Vector2f &position)
{
    UI_window::setPosition(position);
    for (auto& [z_index, elem] : item_elements)
    {
        elem->set_parent_coords(sf::Vector2f(Frame_scale.left, Frame_scale.top));
    }
}

void InventoryDraw::setOrigin(const Vector2f &origin)
{
    UI_window::setOrigin(origin);
    for (auto& [z_index, elem] : item_elements)
    {
        elem->set_parent_coords(sf::Vector2f(Frame_scale.left, Frame_scale.top));
    }
}

void InventoryDraw::setScale(const Vector2f &factors)
{
    UI_window::setScale(factors);
    for (auto& [z_index, elem] : item_elements)
    {
        elem->set_parent_coords(sf::Vector2f(Frame_scale.left, Frame_scale.top));
    }
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
    if (bag->is_inventory_changed() && displayed)
    {
        view_inventory();
        bag->set_inventory_changed(false);
    }
}