#include "UI_window.h"

#include "ResourceLoader.h"
#include "Scene.h"
#include "Scene_Field.h"
#include "SceneController.h"

UI_window::UI_window(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed) :
    UI_element(name, UIFrame, parent, resload),
    ParentFrame(UIFrame), isFramed(is_framed), pressed(false), clicked_child(NULL)
{
    window_view = sf::View(sf::FloatRect(UIFrame));

    displayed = true;
}

// reloads interface and other info from config
void UI_window::load_config(nlohmann::json j)
{
    Frame_scale = ParentFrame;
    sf::Vector2i windowsize = ParentFrame.getSize();

    std::shared_ptr<sf::Texture> back;
    sf::Vector2f coords;
    sf::Vector2f size(windowsize);
    sf::Vector2f origin;

    if (j.contains("position"))
    {
        sf::Vector2f coords, size, origin;

        coords = get_coords_from_json(j["position"], windowsize);
        coords += sf::Vector2f(ParentFrame.getPosition());
        size = get_size_from_json(j["position"], windowsize);
        origin = get_origin_from_json(j["position"], sf::Vector2f(windowsize));

        setFrame(sf::IntRect(sf::Vector2i(coords - origin), sf::Vector2i(size)));
    }

    windowsize = Frame_scale.getSize();
    if (j.contains("texture"))
    {
        back = resource_manager->getUITexture(j["texture"].get<std::string>());

        sf::IntRect back_rect(sf::Vector2i(0, 0), windowsize);
        std::shared_ptr<UI_button> background = std::make_shared<UI_button>(
            name + " background", back_rect, parent_scene, resource_manager, back
        );
        int transparency = j.value("transparency", 255);
        background->setColor(sf::Color(255, 255, 255, transparency));
        addElement(background, -1);
    }

    // create and place buttons
    for (nlohmann::json j2 : j["UI elements"])
    {
        back = resource_manager->getUITexture(j2["texture"]);

        std::string element_name = j2["type"].get<std::string>() + ":" + j2["texture"].get<std::string>();

        sf::Vector2f texsize(back->getSize());

        sf::Vector2f coords = get_coords_from_json(j2, windowsize);

        sf::Vector2f size = get_size_from_json(j2, windowsize);
        size = save_aspect_ratio(size, texsize);
        sf::Vector2f origin = get_origin_from_json(j2, texsize);

        IntRect posrect(coords.x, coords.y, size.x, size.y);

        std::shared_ptr<Animation> spritesheet = std::make_shared<Animation>();
        spritesheet->addSpriteSheet(back);
        spritesheet->addFrame(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texsize)), 0);

        // if button - we need second texture (pushed) and callback
        if (j2["type"] == "button")
        {
            std::shared_ptr<sf::Texture> back_pressed;
            if (j2["texture_pressed"].is_string())
                back_pressed = resource_manager->getUITexture(j2["texture_pressed"]);
            else
                back_pressed = back;

            sf::Vector2f texsize_pressed(back_pressed->getSize());

            spritesheet->addSpriteSheet(back_pressed);
            spritesheet->addFrame(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(texsize_pressed)), 1);

            std::function<void()> callback;
            std::string callback_name = j2["callback"]["name"];
            if (callback_name == "change scene")
            {
                callback = create_change_scene_callback(*parent_scene, j2["callback"]["scene to"].get<std::string>());
            }
            else if (callback_name == "change field")
            {
                Scene_Field* this_field = dynamic_cast<Scene_Field*>(parent_scene);
                if (this_field == nullptr)
                {
                    loading_logger->error("Adding illegal callback to button: {}", j2["callback"]["name"]);
                }
                callback = create_change_field_callback(*this_field, j2["callback"]["field to"].get<int>());
            }
            else if (callback_name == "close window")
            {
                callback = create_window_closed_callback(*(parent_scene->get_scene_controller().get_current_window()));
            }
            else if (callback_name == "close UI_window")
            {
                callback = close_UI_window_callback(*parent_scene, name);
            }
            else
            {
                loading_logger->warn("Unknown callback {}", callback_name);
            }

            std::shared_ptr<UI_button> button = std::make_shared<UI_button>(element_name, posrect, parent_scene, resource_manager, spritesheet, callback);
            button->setOrigin(origin);

            /// TEMP
            button->setText(element_name);
            button->setAlign("centered");
            button->setFont("Lobster-Regular");
            button->setCharSize(24);

            addElement(button, 2);

            loading_logger->trace(
                "Added button {} to scene with coords {:.0f}x{:.0f} and origin {:.0f}x{:.0f}",
                element_name, coords.x, coords.y, origin.x, origin.y
            );
        }
    }
}

// sets displayed for self and child elements
void UI_window::show(bool disp)
{
    displayed = disp;
    for (auto& [z_index, elem] : elements)
        elem->displayed = disp;
}

// gets window element by name (recursive)
std::shared_ptr<UI_window> UI_window::get_subwindow(std::string name)
{
    std::shared_ptr<UI_window> res = std::shared_ptr<UI_window>(nullptr);
    for (auto& [z_index, elem] : elements)
    {
        std::shared_ptr<UI_window> subwindow = std::dynamic_pointer_cast<UI_window>(elem);
        if (subwindow)
        {
            res = subwindow;
            if (res && res->name == name)
                break;
            res = subwindow->get_subwindow(name);
            if (res && res->name == name)
                break;
        }
    }
    return res;
}

// adds UI_element into set with z_index
void UI_window::addElement(std::shared_ptr<UI_element> new_element, int z_index)
{
    displayed = true;

    new_element->z_index = z_index;
    new_element->set_parent_coords(sf::Vector2f(Frame_scale.getPosition()));
    elements.insert(std::make_pair(z_index, new_element));
}

void UI_window::deleteElement(std::shared_ptr<UI_element> element)
{
    elements.erase(std::make_pair(element->z_index, element));
}

// mouse hover check
bool UI_window::contains(sf::Vector2f cursor)
{
    bool res = UI_element::contains(cursor);

    // not only if this->contains cursor, but any child counts
    for (auto& [z_index, elem] : elements)
        if (elem->contains(cursor))
            res = true;

//    input_logger->trace("Window {} at {}x{} contains? {}", name, cursor.x, cursor.y, res);

    return res;
}

bool UI_window::is_clicked() const
{
    return pressed;
}

// pushes hovered element
void UI_window::push_click(sf::Vector2f cursor, bool controls_blocked)
{
        input_logger->trace("Window {} clicked at {}x{}", name, cursor.x, cursor.y);

    // if cursor missed, then don't click, and release focus
    if (!contains(cursor))
    {
        set_focus(false);

        for (auto pi : elements)
            pi.second->set_focus(false);
    }
    else
    {
        set_focus(true);
        pressed = true;

        // find the clicked child and remember it
        // do in reverse order to sort by reverse z-index (the bigger z-index, the bigger is click interception priority)
        for (auto& [z_index, element] : std::ranges::reverse_view(elements))
        {
            if (element->contains(cursor))
            {
                element->push_click(cursor, controls_blocked);
                clicked_child = element;
                break;
            }
        }
    }
}

// releases push (and invokes callback if hovered element is pushed)
void UI_window::release_click(sf::Vector2f cursor, bool controls_blocked, bool skip_action)
{
    input_logger->trace("Window {} released from {}x{}", name, cursor.x, cursor.y);

    pressed = false;
    // release_click doens't trigger any focus logic, so only release_click pressed child
    // transfer event to pushed child. If <skip_action>, no callback is called regardless of <cursor>
    if (clicked_child)
    {
        clicked_child->release_click(cursor, controls_blocked, skip_action);
        clicked_child = NULL;
    }
}

// return number of elements
std::size_t UI_window::get_elements_size() const
{
    return elements.size();
}

// overriding Transformable methods to support nested windows
void UI_window::setPosition(const Vector2f &position)
{
    UI_element::setPosition(position);
    for (auto& [z_index, elem] : elements)
    {
        elem->set_parent_coords(sf::Vector2f(Frame_scale.left, Frame_scale.top));
    }
}

void UI_window::setOrigin(const Vector2f &origin)
{
    UI_element::setOrigin(origin);
    for (auto& [z_index, elem] : elements)
    {
        elem->set_parent_coords(sf::Vector2f(Frame_scale.left, Frame_scale.top));
    }
}

void UI_window::setScale(const Vector2f &factors)
{
    UI_element::setScale(factors);
    for (auto& [z_index, elem] : elements)
    {
        elem->set_parent_coords(sf::Vector2f(Frame_scale.left, Frame_scale.top));
    }
}

// before drawing send child elements to sort by z-index
void UI_window::draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap) const
{
    UI_element::draw_to_zmap(zmap);
    for (auto& [z_index, element] : elements)
        element->draw_to_zmap(zmap);
}

void UI_window::update(sf::Event& event)
{
    if (event.type == sf::Event::MouseMoved)
        for (auto& [z_index, element] : elements)
            if (element->is_hovered() || element->contains(sf::Vector2f(event.mouseMove.x, event.mouseMove.y)))
                element->update(event);
}

void UI_window::update(Time deltaTime)
{
    for (auto& [z_index, element] : elements)
        element->update(deltaTime);
}

// override draw to enable window_view
void UI_window::draw(RenderTarget& target, RenderStates states) const
{
    if (displayed)
    {
        sf::View previous_view = target.getView();
        target.setView(window_view);

        UI_element::draw(target, states);

        target.setView(previous_view);
    }
}