#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include <vector>
#include "UI_element.h"

class UI_layout
{
    public:
        UI_layout();

    private:
        std::vector <UI_element*> elements;

};

#endif // UI_LAYOUT_H
