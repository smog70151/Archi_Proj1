#include <iostream>
#include <string>
#include <fstream>
#include "image_content.h"
#include "error_detect.h"

using namespace std;

//handle the inst
int trans_inst(unsigned int inst, int no_inst, int cyc);

//error file var
extern fstream report_error;

