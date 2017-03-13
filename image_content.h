#include <iostream>
#include <string>


using namespace std;


//reg data
extern int reg_pre[35];
extern int reg_cur[35];

//bounding pc
extern unsigned int min_pc;
extern unsigned int max_pc;
extern unsigned int init_pc;
extern unsigned int init_max_pc;

//inst_memory(iimage)
extern unsigned int no_inst_data;
extern unsigned int* inst_data;
extern unsigned int* inst_pc_addr;

//data_memory
extern unsigned int no_data_data;
extern unsigned int* data_data;
extern unsigned int* data_pc_addr;

extern unsigned int address; //26 bits
extern unsigned int immediate; //16 bits
extern unsigned int opcode, funct; //6 bits
extern unsigned int rt, rs, rd, shamt; //5 bits
extern unsigned int inst_pre; //to detect the overwrite HI-LO
extern int flag_hi;
extern int flag_lo;

