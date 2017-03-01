#include <iostream>
#include <iomanip>
#include <fstream>
#include <bitset>
#include <string>

#include "image_content.h"

using namespace std;

unsigned int c2i_inst_data(unsigned int inst, int pow,unsigned char value) // c2i -> ( alter char to 32'b integer )
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

void read_inst()  //read iimage.bin and store the inst step by step
{
    fstream iimage;
    iimage.open("iimage.bin",ios::in);
    unsigned char c;
    unsigned int inst;
    unsigned int count;
    unsigned int pc_addr_init;

    //initialize PC_addr (iimage_0)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=c2i_inst_data(inst,4-i,c);
    }
    //store the initial PC address
    pc_addr_init=inst;

    //number of instructions (iimage_1)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=c2i_inst_data(inst,4-i,c);
    }
    //store the no of inst and create 2 dynamic array, which store the inst and the PC addr
    no_inst_data=inst;
    inst_data = new unsigned int[no_inst_data];
    inst_pc_addr = new unsigned int[no_inst_data];

    //(iimage_2~end)
    count = inst;
    while(count)
    {
        //read the rest part of the iimage file
        for(int i=0; i<4; i++)
        {
            if(i==0) inst = 0;
            iimage.read((char*)&c,sizeof(char));
            inst=c2i_inst_data(inst,4-i,c);
        }
        //store the message
        inst_data[no_inst_data-count]=inst;
        inst_pc_addr[no_inst_data-count]=pc_addr_init+(no_inst_data-count)*4;

        count--;
        if((inst>>26)==63) //terminate the program with the inst "halt"
            break;
    }
    iimage.close();
}

void Init_reg()
{
    for(int i = 0; i<35; i++)
        reg_pre[i]=(1<<31); //2^31
}

void Snapshot(int cyc)
{
    cout << "cycle " << cyc << endl;
    for(int i=0; i<35; i++) // print the ans in the ordered list
    {
        if(reg_pre[i]==reg_cur[i]) //if the reg did not alter, it will skip.
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


int main()
{

    Init_reg(); // Initialize reg_pre
    read_inst(); //read instruction file and store the message
    read_data(); //read data file and analyze the inst

    return 0;

}
