#include "image_content.h"
#include <iostream>
#include <bitset>

using namespace std;

//reg data
unsigned int reg_pre[35];
unsigned int reg_cur[35];

//inst_memory(iimage)
unsigned int no_inst_data;
unsigned int* inst_data;
unsigned int* inst_pc_addr;

//data_memory(dimage)
unsigned int no_data_data;
unsigned int* data_data;
unsigned int* data_pc_addr;

