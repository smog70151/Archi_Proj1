#include <iostream>
#include <string>
#include <fstream>
#include "image_content.h"

using namespace std;

//error file var
extern fstream report_error;

//alter it into 1 if detect error occur
extern int error_halt;

//detect the error in the following order
void detect_reg0(int cyc);
void detect_ovf(int cyc);
void detect_ovr(int cyc);
void detect_D_mem(int mem, int cyc);
void detect_misaligned(int mem, int cyc, int byte);