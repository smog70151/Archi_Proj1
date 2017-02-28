#include "register_content.h"

using namespace std;

void snapshot :: cycle(int no_cyc)
{
    cout << "cycle " << no_cyc << endl;
    if(no_cyc==0)
    {
        for(int i=0; i<32; i++)
            cout << "$" << setw(2) << setfill('0') << i << ": 0x00000000" << endl;
        cout << "HI: 0x00000000" << endl;
        cout << "LO: 0x00000000" << endl;
        cout << "PC: 0x" << instruction[34] << endl;
    }
    else
    {
        for(int i=0; i<35; i++)
        {
            if(cyc[i]==no_cyc&&i<32)
                cout << "$" << setw(2) << setfill('0') << i << ": 0x" << instruction[i] << endl;
            else if(cyc[i]==no_cyc&&i==32)
                cout << "HI: 0x" << instruction[i] << endl;
            else if(cyc[i]==no_cyc&&i==33)
                cout << "LO: 0x" << instruction[i] << endl;
            else if(cyc[i]==no_cyc&&i==34)
                cout << "PC: 0x" << instruction[i] << endl;
        }
    }
    cout << endl << endl;
}

void snapshot :: alter_inst(int no_cyc, int no_inst, int bit_place, char bit_value)
{
    cyc[no_inst]=no_cyc;
    if(bit_place==0)
    {
        instruction[no_inst]="";
    }
    if(bit_place<=3)
    {
        instruction[no_inst]+=((bit_value/16)>=10)?'A'+(bit_value/16)-10:'0'+(bit_value/16);
        instruction[no_inst]+=((bit_value%16)>=10)?'A'+(bit_value%16)-10:'0'+(bit_value%16);
    }
    else return;
}
