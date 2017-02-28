#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class snapshot
{
public:
    string instruction[35];
    int cyc[35]={0};

    void cycle(int no_cyc);
    void alter_inst(int no_cyc, int no_inst, int bit_place, char bit_value);
};
