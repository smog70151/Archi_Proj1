#include "image_content.h"
#include <iostream>

using namespace std;

//reg data
int reg_pre[35];
int reg_cur[35];

//inst_memory(iimage)
unsigned int no_inst_data;
unsigned int* inst_data;
unsigned int* inst_pc_addr;

//data_memory(dimage)
unsigned int no_data_data;
unsigned int* data_data;
unsigned int* data_pc_addr;

unsigned int address; //26 bits
unsigned int immediate; //16 bits
unsigned int opcode, funct; //6 bits
unsigned int rt, rs, rd, shamt; //5 bits
unsigned int inst_pre; //to detect the overwrite HI-LO
int flag_hi;
int flag_lo;