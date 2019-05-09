#include <iostream>
#include "Cell.h"

using namespace std;

class Field
{
public:
    vector < vector < Cell > > cells;
    int width, height;

    Field();
    Field(int width, int height);
};
