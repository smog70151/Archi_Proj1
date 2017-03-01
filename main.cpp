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

    Init_reg(); // Initialize reg_pre

    //initialize cyc 0 (iimage_0)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=inst_data(inst,4-i,c);
    }

    //number of instructions (iimage_1)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=inst_data(inst,4-i,c);
    }

    //(iimage_2~end)
    count = inst;
    while(count)
    {
        for(int i=0; i<4; i++)
        {
            if(i==0) inst = 0;
            iimage.read((char*)&c,sizeof(char));
            inst=inst_data(inst,4-i,c);
        }
        count--;
        if((inst>>26)==63)
            break;
    }
    iimage.close();
    return 0;

}

void Init_reg()
{
    for(int i = 0; i<35; i++)
        reg_pre[i]=1<<31; //2^31
}

void Snapshot(int cyc)
{
    cout << "cycle " << cyc << endl;
    for(int i=0; i<35; i++)
    {
        if(reg_pre[i]==reg_cur[i])
            continue;
        else
        {
            switch(i)
            {
            case 34:  //format = PC: 0x_0000_0000
                cout << "PC: 0x" << hex << reg_cur[34] << endl;
                break;
            case 33:  //format = LO: 0x_0000_0000
                cout << "LO: 0x" << hex << reg_cur[33] << endl;
                break;
            case 32:  //format = HI: 0x_0000_0000
                cout << "HI: 0x" << hex << reg_cur[32] << endl;
                break;
            default:  //format = $10: 0x_0000_0000
                 cout << "$" << setw(2) << setfill('0') << i << ": 0x" << hex << reg_cur[i] << endl;
            }
        }
    }
    cout << endl << endl;
}
