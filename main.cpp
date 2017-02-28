#include <iostream>
#include <fstream>
#include <string>

#include "register_content.h"

using namespace std;

int main()
{
    fstream iimage;
    snapshot snap;
    iimage.open("iimage.bin",ios::in);
    unsigned char c;
    int i=0;

    //initialize cyc 0 (iimage_0)
    for(int i=0; i<4; i++)
    {
        iimage.read((char*)&c,sizeof(char));
        snap.alter_inst(0,34,i,c);
    }
    snap.cycle(0);

    //(iimage_1)
    snap.alter_inst(1,34,4,c);
    snap.instruction[34][7]+=4;
    for(int i=0; i<4; i++)
    {
        iimage.read((char*)&c,sizeof(char));
    }
    snap.cycle(1);

    //(iimage_2~end)
    while(iimage.read((char*)&c,sizeof(char)))
    {
        for(int i=0; i<4; i++)
        {
            iimage.read((char*)&c,sizeof(char));
            //snap.alter_inst(34,i,c);
        }
        //snap.cycle(0);
    }
    iimage.close();
    return 0;

}
