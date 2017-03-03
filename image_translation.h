#include <iostream>
#include <string>
#include "image_content.h"

using namespace std;

//handle the inst
int trans_inst(unsigned int inst, int no_inst);
int sign(unsigned int data, string option);

extern unsigned int address; //26 bits
extern unsigned int immediate; //16 bits
extern unsigned int opcode, funct; //6 bits
extern unsigned int rt, rs, rd, shamt; //5 bits
