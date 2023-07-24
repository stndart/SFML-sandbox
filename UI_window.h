#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include <set>
#include <string>
#include <memory>

#include "UI_element.h"
#include "UI_button.h"

class UI_window : public UI_element
{
    private:
        // list of UI_elements with z-indexes
        std::set<std::pair<int, std::shared_ptr<UI_element> > > elements;
        // checks if frame (with _, o, x buttons) is displayed
        bool isFramed; /// NOT IMPLEMENTED
        // checks if pushed by mouse
        bool pressed;
        // remember clicked child to invoke release on him later
        std::shared_ptr<UI_element> clicked_child;

        // logger is inherited and constructed in parent constructor

    public:
        UI_window(std::string name, sf::IntRect UIFrame, Scene* parent, bool is_framed = false);

        // push UI_element into list
        void addElement(std::shared_ptr<UI_element> new_element, int z_index = 0);
        // delete element by pointer
        void deleteElement(std::shared_ptr<UI_element> element);

        // we override contains since window is no more rectangle: it contains overlapping and outbordering children elements
        bool contains(sf::Vector2f cursor);

        bool is_clicked() const;
        // pushes hovered element
        void push_click(sf::Vector2f cursor) override;
        // releases push (and invokes callback if hovered element is pushed). If <skip_action> then doesn't invoke callback
        void release_click(sf::Vector2f cursor, bool skip_action=false) override;

        // return number of elements
        std::size_t get_elements_size() const;

        // overriding Drawable methods
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // UI_WINDOW_H
