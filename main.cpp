#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "image_content.h"

using namespace std;

unsigned int inst_data(unsigned int inst, int pow,unsigned char value)
{
    if(pow==4)
        inst=(value>>4<<28)+((value%16)<<24);
    else if(pow==3)
        inst+=(value>>4<<20)+((value%16)<<16);
    else if(pow==2)
        inst+=(value>>4<<12)+((value%16)<<8);
    else if(pow==1)
        inst+=(value>>4<<4)+value%16;
    return inst;
}

int main()
{
    fstream iimage;
    iimage.open("iimage.bin",ios::in);
    unsigned char c;
    unsigned int inst=0;
    unsigned int count;
    int i=0;

    //initialize cyc 0 (iimage_0)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=inst_data(inst,4-i,c);
    }
    cout << setw(8) << setfill('0')<< hex << inst << endl ;
    //number of instructions (iimage_1)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=inst_data(inst,4-i,c);
    }
    cout << setw(8) << setfill('0')<< hex << inst << endl ;
    count = inst;
    //(iimage_2~end)
    while(count)
    {
        for(int i=0; i<4; i++)
        {
            if(i==0) inst = 0;
            iimage.read((char*)&c,sizeof(char));
            inst=inst_data(inst,4-i,c);
        }
        cout << setw(8) << setfill('0')<< hex << inst << endl ;
        count--;
        if((inst>>26)==63)
            break;
    }
    iimage.close();
    return 0;

}
