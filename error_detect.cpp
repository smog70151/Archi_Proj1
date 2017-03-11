#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
//var repository
#include "image_content.h"

using namespace std;

//var in order to write in the error_dump.rpt
fstream report_error;

//alter it into 1 if detect error occur
int error_halt;

//detect overwrite $0 register
void detect_reg0(int cyc)
{
	reg_cur[0]=0;
	switch(opcode)
	{
	case 0x00 :
		if(rd==0)
			report_error <<  "In cycle " << dec << cyc << ": Write $0 Error" << endl;
	break;
	default :
		if(rt==0)
			report_error <<  "In cycle " << dec << cyc << ": Write $0 Error" << endl;
	break;
	}
}

void detect_ovf(int cyc)
{
	switch(opcode)
	{
	case 0x00 :
		if(funct==0x20)
		{
			if(reg_pre[rs]>=0&&reg_pre[rt]>=0&&reg_cur[rd]<0)
				report_error <<  "In cycle " << dec << cyc << ": Number Overflow" << endl;
			else if(reg_pre[rs]<0&&reg_pre[rt]<0&&reg_cur[rd]>=0)
				report_error <<  "In cycle " << dec << cyc << ": Number Overflow" << endl;
		}
		if(funct==0x22)
		{
            if(reg_pre[rs]>=0&&reg_pre[rt]<0&&reg_cur[rd]<0)
                report_error <<  "In cycle " << dec << cyc << ": Number Overflow" << endl;
            else if(reg_pre[rs]<0&&reg_pre[rt]>=0&&reg_cur[rd]>=0)
                report_error <<  "In cycle " << dec << cyc << ": Number Overflow" << endl;
		}
	break;
	case 0x08 :
		if(reg_cur[rs]>=0&&(int)immediate>=0&&reg_cur[rt]<0)
			report_error <<  "In cycle " << dec << cyc << ": Number Overflow" << endl;
		else if(reg_cur[rs]>=0&&(int)immediate>=0&&reg_cur[rt]<0)
			report_error <<  "In cycle " << dec << cyc << ": Number Overflow" << endl;
	break;
	default :
	break;
	}
}

void detect_ovr(int cyc)
{
	if((inst_pre|0x00000000)!=0x00000000&&((inst_pre%(1<<6)!=0x10)||(inst_pre%(1<<6)!=0x12))) //opcode & funct
	{
		if(flag_hi==0&&flag_lo==0)
		{
			if(reg_cur[32]!=0)
				flag_hi++;
			if(reg_cur[33]!=0)
				flag_lo++;
		}
		else if(flag_hi==1&&flag_lo==0)
		{
			if(reg_cur[32]!=reg_pre[32])
				report_error <<  "In cycle " << dec << cyc << ": Overwrite HI-LO registers" << endl;
			if(reg_cur[33]!=reg_pre[33])
				flag_lo++;
		}
		else if(flag_hi==0&&flag_lo==1)
		{
			if(reg_cur[33]!=reg_pre[33])
				report_error <<  "In cycle " << dec << cyc << ": Overwrite HI-LO registers" << endl;
			if(reg_cur[32]!=reg_pre[32])
				flag_lo++;
		}
		else
		{
			if((reg_cur[32]!=reg_pre[32])||(reg_cur[33]!=reg_pre[33]))
				report_error <<  "In cycle " << dec << cyc << ": Overwrite HI-LO registers" << endl;
		}
	}
}

void detect_D_mem(int mem, int cyc)
{
	if(mem>1023)
	{
		report_error <<  "In cycle " << dec << cyc << ": Address Overflow" << endl;
		error_halt=1;
	}
}

void detect_misaligned(int mem, int cyc, int byte)
{
	switch(byte)
	{
	case 1 :
		break;
	case 2 :
		if(mem%2!=0)
		{
			report_error <<  "In cycle " << dec << cyc << ": Misalignment Error" << endl;
			error_halt=1;
		}
		break;
	case 4 :
		if(mem%4!=0)
		{
			report_error <<  "In cycle " << dec << cyc << ": Misalignment Error" << endl;
			error_halt=1;
		}
		break;
	default :
		break;
	}
}
