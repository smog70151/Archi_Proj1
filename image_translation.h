#include <iostream>
#include <string>
#include <fstream>
#include "image_content.h"

using namespace std;

//handle the inst
int trans_inst(unsigned int inst, int no_inst, int cyc);

//detect the error message
int detect_error(int error, int cyc);

//error file var
extern fstream report_error;

extern unsigned int address; //26 bits
extern unsigned int immediate; //16 bits
extern unsigned int opcode, funct; //6 bits
extern unsigned int rt, rs, rd, shamt; //5 bits


