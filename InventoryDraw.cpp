#include "InventoryDraw.h"

InventoryDraw::InventoryDraw(
    std::string name, sf::IntRect UIFrame,
    sf::Vector2f n_element_size, sf::Vector2i n_element_count,
    Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed) :
    UI_window(name, UIFrame, parent, is_framed)
{
    element_size = n_element_size;
    element_count = n_element_count;
    resource_manager = resload;
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
    sf::IntRect UIFrame(Vector2i(0, 0), Vector2i(n_element_count.x * n_element_count.x, n_element_size.y * n_element_count.y));

    std::shared_ptr<InventoryDraw> idraw = std::make_shared<InventoryDraw>(name, UIFrame, n_element_size, n_element_count, parent, resload, is_framed);
    idraw->linkInventory(bag);
    return idraw;
}

void InventoryDraw::view_inventory()
{
    elements.clear();

    sf::Vector2i invpos(100, 100);
    sf::Vector2i invsize(400, 400);

    std::shared_ptr<UI_button> inv_back = std::make_shared<UI_button>(
        "Inventory background", sf::IntRect(invpos, invsize), parent_scene, resource_manager->getUITexture("background"));
    inv_back->setColor(sf::Color(255, 255, 255, 180));
    addElement(inv_back, 0);

    int row = 0, column = 0;
    for (auto item : bag->getCategory(""))
    {
        sf::Vector2i pos(element_size.x * column, element_size.y * row);
        pos += invpos;

        std::shared_ptr<UI_button> inv_item = std::make_shared<UI_button>(
            "Item: " + item.name, sf::IntRect(pos, sf::Vector2i(element_size)),
            parent_scene, resource_manager->getItemTexture(item.name));
        addElement(inv_item, 1);

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

// update and redraw inventory if needed
void InventoryDraw::update(Time deltaTime)
{
    if (bag->is_inventory_changed())
    {
        loading_logger->warn("redrawing inventory");
        view_inventory();
    }
}