#include <string>

using namespace std;

class Item
{
public:
    string name;
    int amount;

    Item();
    Item(string n, int a);
    Item(string n);
};
