#include <iostream>
#include <string>
#include "image_content.h"


//handle the inst
int trans_inst(unsigned int inst, int no_inst);

extern unsigned int address; //26 bits
extern unsigned int immediate; //16 bits
extern unsigned int opcode, funct; //6 bits
extern unsigned int rt, rs, rd, shamt; //5 bits
