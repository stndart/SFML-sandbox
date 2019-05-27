#include "Field.h"

Field::Field()
{
    width = 20;
    height = 20;
    cells = vector<vector<Cell> > (height, vector<Cell>(width));
}

Field::Field(int width, int height)
{
    width = width;
    height = height;
    cells = vector<vector<Cell> > (height, vector<Cell>(width));
}
