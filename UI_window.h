#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <set>
#include <string>
#include <memory>
#include <fstream>

#include "UI_element.h"
#include "UI_button.h"

#include <nlohmann/json.hpp>

class UI_window : public UI_element
{
private:
    // sprite position and rectangle to hover checks
    sf::IntRect ParentFrame;

    // checks if frame (with _, o, x buttons) is displayed
    bool isFramed; /// NOT IMPLEMENTED
    // checks if pushed by mouse
    bool pressed;
    // remember clicked child to invoke release on him later
    std::shared_ptr<UI_element> clicked_child;

    // to control what part of general interface displays. Also used by scrollbars
    sf::View window_view;

    // logger is inherited and constructed in parent constructor

protected:
    // list of UI_elements with z-indexes
    std::set<std::pair<int, std::shared_ptr<UI_element> > > elements;

public:
    UI_window(std::string name, sf::IntRect UIFrame, Scene* parent, std::shared_ptr<ResourceLoader> resload, bool is_framed = false);

    // reloads interface and other info from config
    virtual void load_config(nlohmann::json j);

    // sets displayed for self and child elements
    virtual void show(bool displayed = true);
    // gets window element by name (recursive)
    virtual std::shared_ptr<UI_window> get_subwindow(std::string name);

    // push UI_element into list
    void addElement(std::shared_ptr<UI_element> new_element, int z_index = 0);
    // delete element by pointer
    void deleteElement(std::shared_ptr<UI_element> element);

    // we override contains since window is no more rectangle: it contains overlapping and outbordering children elements
    bool contains(sf::Vector2f cursor);

    bool is_clicked() const;
    // pushes hovered element
    void push_click(sf::Vector2f cursor, bool controls_blocked) override;
    // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
    void release_click(sf::Vector2f cursor, bool controls_blocked, bool skip_action=false) override;

    // return number of elements
    std::size_t get_elements_size() const;

    // overriding Transformable methods to support nested windows
    void setPosition(const Vector2f &position) override;
    void setOrigin(const Vector2f &origin) override;
    void setScale(const Vector2f &factors) override;

    // before drawing send child elements to sort by z-index
    void draw_to_zmap(std::map<int, std::vector<const Drawable*> > &zmap) const override;

    virtual void update(Time deltaTime) override;
    // override draw to enable window_view
    void draw(RenderTarget& target, RenderStates states) const override;
};

#endif // UI_WINDOW_H
