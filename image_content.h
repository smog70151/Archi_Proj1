#include <iostream>
#include <string>
#include "image_translation.h"

using namespace std;


//reg data
extern int reg_pre[35];
extern int reg_cur[35];

//inst_memory(iimage)
extern unsigned int no_inst_data;
extern unsigned int* inst_data;
extern unsigned int* inst_pc_addr;

//data_memory
extern unsigned int no_data_data;
extern unsigned int* data_data;
extern unsigned int* data_pc_addr;

