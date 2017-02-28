#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

unsigned int inst_data(unsigned int inst, int pow, char value)
{
    if(pow==4)
        inst+=value/16*(16*16*16)*(16*16*16*16)+value%16*(16*16*16)*(16*16*16);
    else if(pow==3)
        inst+=value/16*16*16*16*16*16+value%16*16*16*16*16;
    else if(pow==2)
        inst+=value/16*16*16*16+value%16*16*16;
    else if(pow==1)
        inst+=value/16*16+value%16;
    return inst;
}

int main()
{
    fstream iimage;
    iimage.open("iimage.bin",ios::in);
    unsigned char c;
    unsigned int inst=0;
    int i=0;

    //initialize cyc 0 (iimage_0)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=inst_data(inst,4-i,c);
    }
    cout << setw(8) << setfill('0')<< hex << inst << endl ;
    //(iimage_2~end)
    while(iimage.read((char*)&c,sizeof(char)))
    {
        for(int i=0; i<4; i++)
        {
            iimage.read((char*)&c,sizeof(char));
        }
    }
    iimage.close();
    return 0;

}
