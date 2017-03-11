#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
//var repository
#include "image_content.h"
//error function
#include "error_detect.h"
//dealing with data
#include "image_translation.h"

using namespace std;

//variables
fstream report_cyc;
//function
void Init_reg(); //initialize register
// trans char 2 int (4*8 bits -> 32 bits)
unsigned int c2i_inst_data(unsigned int inst, int pow,unsigned char value); // char 2 unsigned int

//load the data
void read_inst(); //read the iimage.bin and store it into the inst array. Besides, store the PC addr in the same time.
void read_data(); //read the dimage.bin and store it into the data array. Besides, analyze the inst_data synchronously.

//output the outcome
void Snapshot(int cyc); //output the snapshot

int main()
{
    int cyc=0; //current cycle
	int no_inst_cur=0;    // current instruction (control the inst)

    Init_reg(); // Initialize reg_pre
    read_inst(); //read instruction file and store the message
    read_data(); //read data file and analyze the inst

	report_cyc.open("snapshot_mine.rpt",ios::out); //in order to record chase data in every cyc
	report_error.open("error_dump_mine.rpt",ios::out); //in order to chase error in every cyc

	//initialize var
	flag_hi=0;
	flag_lo=0;
	error_halt=0;

	while(1)
	{
		Snapshot(cyc);
		no_inst_cur=trans_inst(inst_data[no_inst_cur], no_inst_cur, cyc+1); //deal with the data, and return next inst
		//cout << "no_inst_cur : " << no_inst_cur << endl;
		if(no_inst_cur==-2||error_halt==1) //detect "halt" instruction
			break;
		reg_cur[34]=inst_pc_addr[no_inst_cur]; //renew the PC address
		cyc++;
	}

	report_cyc.close();
	report_error.close();


    return 0;

}

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
    iimage.open("iimage.bin",ios::in|ios::binary);
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
	reg_cur[34]=pc_addr_init;

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
    }
    iimage.close();
}

void read_data()  //read the dimage.bin and store it into the data array. Besides, analyze the inst_data synchronously.
{
    fstream dimage;
    dimage.open("dimage.bin",ios::in|ios::binary);
    unsigned char c;
    unsigned int data;
    unsigned int count;

    //read the first line in the file (dimage_0)
    for(int i=0; i<4; i++)
    {
        if(i==0) data = 0;
        dimage.read((char*)&c,sizeof(char));
        data=c2i_inst_data(data,4-i,c);
    }
    //store the reg[29]($sp) data
    reg_cur[29]=data;

    //number of data (dimage_1)
    for(int i=0; i<4; i++)
    {
        if(i==0) data = 0;
        dimage.read((char*)&c,sizeof(char));
        data=c2i_inst_data(data,4-i,c);
    }
    //store the no of data and create 2 dynamic array, which store the data and the PC addr
    no_data_data=data;
    data_data = new unsigned int[1024];
    data_pc_addr = new unsigned int[no_data_data];

    //(dimage_2~end)
    count = no_data_data;
	int i = 0;
    while(count)
    {
        //read the rest part of the dimage file
		for(int i=0; i<4; i++)
		{
			if(i==0) data = 0;
			dimage.read((char*)&c,sizeof(char));
			data=c2i_inst_data(data,4-i,c);
		}
        //store the message
        data_data[4*(no_data_data-count)]  =(data & 0xff000000)>>24;
		data_data[4*(no_data_data-count)+1]=(data & 0x00ff0000)>>16;
		data_data[4*(no_data_data-count)+2]=(data & 0x0000ff00)>>8;
		data_data[4*(no_data_data-count)+3]= data & 0x000000ff;


        data_pc_addr[no_data_data-count]=0;

        count--;
    }
    dimage.close();
}

void Init_reg()
{
    for(int i = 0; i<35; i++)
    {
        reg_pre[i]=(1<<31); //2^31
        reg_cur[i]=0;
    }
}

void Snapshot(int cyc)
{

    report_cyc << "cycle " << dec << cyc << endl;
    for(int i=0; i<35; i++) // print the ans in the ordered list
    {
        if(reg_pre[i]==reg_cur[i]) //if the reg did not alter, it will skip.
            continue;
        else
        {
            switch(i)
            {
            case 34:  //format = PC: 0x_0000_0000
                report_cyc << "PC: 0x" << setw(8) << setfill('0') << hex << uppercase << reg_cur[34] << endl;
                break;
            case 33:  //format = LO: 0x_0000_0000
                report_cyc << "$LO: 0x" << setw(8) << setfill('0') << hex << uppercase << reg_cur[33] << endl;
                break;
            case 32:  //format = HI: 0x_0000_0000
                report_cyc << "$HI: 0x" << setw(8) << setfill('0') << hex << uppercase << reg_cur[32] << endl;
                break;
            default:  //format = $10: 0x_0000_0000
                report_cyc << "$" << setw(2) << setfill('0') << dec << i << ": 0x" << setw(8) << setfill('0') << hex << uppercase << reg_cur[i] << endl;
				break;
            }
        }
    }
    report_cyc << endl << endl;
    for(int i=0; i<35; i++)
        reg_pre[i]=reg_cur[i];
}
