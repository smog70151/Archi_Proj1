#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "image_content.h"

using namespace std;

unsigned int address; //26 bits
unsigned int immediate; //16 bits
unsigned int opcode, funct; //6 bits
unsigned int rt, rs, rd, shamt; //5 bits
unsigned int inst_pre; //to detect the overwrite HI-LO
int flag_hi;
int flag_lo;

fstream report_error;

int detect_error(int error, int cyc)
{
	/*
	error type :
	1.Write to register $0
	2.Number overflow
	3.Overwrite HI-LO registers
	4.Memory address overflow
	5.Data misaligned
	*/
	if(error==1) // Write to register $0
	{
		switch(opcode)
		{
            case 0x00 :
				if(rd!=0)
					return 0;
				else
				{
					report_error <<  "In cycle " << dec << cyc << ": Write $0 Error" << endl;
					return 1;
				}
			break;
			default :
				if(rt!=0)
					return 0;
				else
				{
					report_error <<  "In cycle " << dec << cyc << ": Write $0 Error" << endl;
					return 1;
				}
			break;
		}
	}
	else if(error==2) // Number overflow
	{
		switch(opcode)
		{
			case 0x00 :
				if(funct==0x20
				|| funct==0x22)
				{
					if(reg_cur[rs]>=0&&reg_cur[rt]>=0&&reg_cur[rd]<0)
						report_error <<  "In cycle " << dec << cyc << ": Number Overflow" << endl;
					else if(reg_cur[rs]<0&&reg_cur[rt]<0&&reg_cur[rd]>=0)
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
	else if(error==3) // Overwrite HI-LO registers
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
	return 0;
}

int detect_D_mem(int mem, int cyc)
{
	if(mem>1023)
	{
		report_error <<  "In cycle " << dec << cyc << ": Address Overflow" << endl;
		return 1;
	}
	else
		return 0;
}

int detect_misaligned(int mem, int cyc, int byte)
{
	switch(byte)
	{
	case 1 :
		return 0;
		break;
	case 2 :
		if(mem%2!=0)
		{
			report_error <<  "In cycle " << dec << cyc << ": Misalignment Error" << endl;
			return 1;
		}
		else
			return 0;
		break;
	case 4 :
		if(mem%4!=0)
		{
			report_error <<  "In cycle " << dec << cyc << ": Misalignment Error" << endl;
			return 1;
		}
		else
			return 0;
		break;
		break;
	default :
		return 0;
		break;
	}
}

//handle the inst
int trans_inst(unsigned int inst, int no_inst, int cyc)
{
	unsigned int pc_addr; //a temp pc address to seek for the next inst :)
	long long int temp_rs, temp_rt;
    //cout << "instrution : " << hex << inst << endl;

    opcode = inst>>26; // trans opcode in the beginning to determine the next step
	if(opcode!=0x00)
		inst_pre=inst;
    cout << "opcode : " << hex << opcode << endl;
    switch(opcode)
    {
        //R-type instructions op(6)_rs(5)_rt(5)_rd(5)_shamt(5)_funct(6)
        //opcode = 0x00
        case 0x00 :
            funct = inst%(1<<6);
			cout << "funct : " << hex << funct << " cyc " << (cyc-1) << endl;
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			rd = (inst>>11)%(1<<5);
			if(funct!=0x18||funct!=0x19)
				inst_pre=inst;
            switch(funct)
            {
                case 0x20 : //add $d = $s + $t
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=reg_pre[rs]+reg_pre[rt];
					detect_error(2,cyc);
                    return ++no_inst;
					break;
                case 0x21 : //addu $d = $s + $t(unsigned, no overflow exception)
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=(unsigned int)reg_pre[rs]+(unsigned int)reg_pre[rt];
                    return ++no_inst;
					break;
                case 0x22 : //sub $d = $s - $t
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=reg_pre[rs]-reg_pre[rt];
					detect_error(2,cyc);
                    return ++no_inst;
					break;
                case 0x24 : //and $d = $s & $t
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=reg_pre[rs]&reg_pre[rt];
                    return ++no_inst;
					break;
                case 0x25 : //or $d = $s | $t
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=reg_pre[rs]|reg_pre[rt];
                    return ++no_inst;
					break;
				case 0x26 : //xor $d = $s ^ $t
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=reg_pre[rs]^reg_pre[rt];
                    return ++no_inst;
					break;
				case 0x27 : //nor $d = ~ ($s | $t)
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=~(reg_pre[rs]|reg_pre[rt]);
                    return ++no_inst;
					break;
				case 0x28 : //nand $d = ~($s & $t)
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=~(reg_pre[rs]&reg_pre[rt]);
                    return ++no_inst;
					break;
				case 0x2a : //slt $d = ($s < $t), signed comparison
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					reg_cur[rd]=(reg_pre[rs]<reg_pre[rt]);
                    return ++no_inst;
					break;
				case 0x00 : //sll $d = $t << C
					shamt = (inst>>6)%(1<<5);
					if(detect_error(1,cyc)&& inst!=0x00000000) // error : write to register $0
						return++no_inst;
					reg_cur[rd]= reg_pre[rt]<<shamt;
                    return ++no_inst;
					break;
				case 0x02 : //srl $d = $t >> C
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					shamt = (inst>>6)%(1<<5);
					reg_cur[rd]= reg_pre[rt]>>shamt;
                    return ++no_inst;
					break;
				case 0x03 : //sra $d = $t >> C, with sign bit shifted in
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
					shamt = (inst & 0x00000400)>>10;
					reg_cur[rd]= (reg_pre[rt]>>shamt);
                    return ++no_inst;
					break;
				case 0x08 : //jr PC=$s
					pc_addr=reg_pre[rs];
					for(unsigned int i = 0; i<no_inst_data; i++)
						if(pc_addr==inst_pc_addr[i])
							return no_inst=i;
					break;
				case 0x18 : //mult {Hi || Lo} = $s * $t
					if(reg_pre[rs] & 0x80000000)
						temp_rs=reg_pre[rs] | 0xffffffff00000000;
					else
						temp_rs=reg_pre[rs] | 0x00000000ffffffff;
					if(reg_pre[rt] & 0x80000000)
						temp_rt=reg_pre[rt] | 0xffffffff00000000;
					else
						temp_rt=reg_pre[rt] | 0x00000000ffffffff;
					reg_cur[32] = (temp_rs*temp_rt)>>32;
					reg_cur[33] = (temp_rs*temp_rt) & 0x00000000ffffffff;
					detect_error(3,cyc); // detect HI/LO OVF
                    return ++no_inst;
					break;
				case 0x19 : //multu {Hi || Lo} = $s * $t (unsigned, no overflow exception)
					if(reg_pre[rs] & 0x80000000)
						temp_rs=reg_pre[rs] & 0x00000000ffffffff;
					else
						temp_rs=reg_pre[rs] & 0x00000000ffffffff;
					if(reg_pre[rt] & 0x80000000)
						temp_rt=reg_pre[rt] & 0x00000000ffffffff;
					else
						temp_rt=reg_pre[rt] & 0x00000000ffffffff;
					reg_cur[32] = (temp_rs*temp_rt)>>32;
					reg_cur[33] = (temp_rs*temp_rt) & 0x00000000ffffffff;
					detect_error(3,cyc); // detect HI/LO OVF
                    return ++no_inst;
					break;
				case 0x10 : //mfhi $d = Hi
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
                    reg_cur[rd] = reg_pre[32];
					return ++no_inst;
					break;
				case 0x12 : //mflo $d = Lo
					if(detect_error(1,cyc)) // error : write to register $0
						return++no_inst;
                    reg_cur[rd] = reg_pre[33];
					return ++no_inst;
					break;
				default :
					return -1; // error
					break;
            }
            break;
        //J-type instructions
        //opcode(6)_address(26)
        case 0x02 : //j PC = (PC+4)[31:28] | 4*C(unsigned)
			address=inst%(1<<26);
			pc_addr=((inst_pc_addr[no_inst]+4)>>27)|(4*address);
            for(unsigned int i = 0; i<no_inst_data; i++)
				if(pc_addr==inst_pc_addr[i])
					return no_inst=i;
			break;
        case 0x03 : //jal $31 = PC + 4;PC = (PC+4)[31:28] | 4*C(unsigned)
			address=inst%(1<<26);
			reg_cur[31]=inst_pc_addr[no_inst]+4;
			pc_addr=((inst_pc_addr[no_inst]+4)>>27)|(4*address);
            for(unsigned int i = 0; i<no_inst_data; i++)
				if(pc_addr==inst_pc_addr[i])
					return no_inst=i;
            break;
		//I-type instructions
		//opcode(6)_rs(5)_rt(5)_immediate(16)
		case 0x08 : //addi $t = $s + C(signed)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			reg_cur[rt] = reg_cur[rs] + (int)immediate;
			detect_error(2,cyc);

			return ++no_inst;
			break;
		case 0x09 : //addiu $t = $s + C(unsigned, no overflow exception)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			reg_cur[rt] = reg_cur[rs] + immediate;
			return ++no_inst;
			break;
		case 0x23 : //lw $t = 4 bytes from Memory[$s + C(signed)]
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate+3,cyc))
				return -2;
			if(detect_misaligned(reg_pre[rs]+(int)immediate+3,cyc,4)) //detect the misaligned
				return -2;
			reg_cur[rt] =(data_data[reg_pre[rs]+(int)immediate]  <<24)
						+(data_data[reg_pre[rs]+(int)immediate+1]<<16)
						+(data_data[reg_pre[rs]+(int)immediate+2]<< 8)
						+(data_data[reg_pre[rs]+(int)immediate+3]    );
			return ++no_inst;
			break;
		case 0x21 : //lh $t = 2 bytes from Memory[$s + C(signed)], signed
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate+1,cyc))
				return -2;
			if(detect_misaligned(reg_pre[rs]+(int)immediate+1,cyc,2)) //detect the misaligned
				return -2;
			reg_cur[rt] =(data_data[reg_pre[rs]+(int)immediate  ]<<8)
						+ data_data[reg_pre[rs]+(int)immediate+1];
			if(reg_cur[rt] & 0x00008000)
				reg_cur[rt] = reg_cur[rt] | 0xffff8000;
			return ++no_inst;
			break;
		case 0x25 : //lhu $t = 2 bytes from Memory[$s + C(signed)], unsigned
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate+1,cyc))
				return -2;
			if(detect_misaligned(reg_pre[rs]+(int)immediate+1,cyc,2)) //detect the misaligned
				return -2;
			reg_cur[rt] =(data_data[reg_pre[rs]+(int)immediate]<<8)
						+ data_data[reg_pre[rs]+(int)immediate+1]  ;
			return ++no_inst;
			break;
		case 0x20 : //lb $t = Memory[$s + C(signed)], signed
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate,cyc))
				return -2;
			reg_cur[rt] = data_data[reg_pre[rs]+(int)immediate];
			if(reg_cur[rt] & 0x0000080)
				reg_cur[rt] =reg_cur[rt] | 0xffffff00;
			return ++no_inst;
			break;
		case 0x24 : //lbu $t = Memory[$s + C(signed)], unsigned
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate,cyc))
				return -2;
			reg_cur[rt] = data_data[reg_pre[rs]+(int)immediate];
			return ++no_inst;
			break;
		case 0x2b : //sw 4 bytes from Memory[$s + C(signed)] = $t
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate+3,cyc))
				return -2;
			if(detect_misaligned(reg_pre[rs]+(int)immediate+3,cyc,4)) //detect the misaligned
				return -2;
			data_data[reg_pre[rs]+(int)immediate  ]=reg_pre[rt]>>24 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+1]=reg_pre[rt]>>16 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+2]=reg_pre[rt]>> 8 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+3]=reg_pre[rt]     & 0x000000ff;
			return ++no_inst;
			break;
		case 0x29 : //sh 2 bytes from Memory[$s + C(signed)] = $t & 0x0000FFFF
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate+1,cyc))
				return -2;
			if(detect_misaligned(reg_pre[rs]+(int)immediate+1,cyc,2)) //detect the misaligned
				return -2;
			data_data[reg_pre[rs]+(int)immediate  ]=reg_pre[rt]>> 8 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+1]=reg_pre[rt]     & 0x000000ff;
			return ++no_inst;
			break;
		case 0x28 : //sb Memory[$s + C(signed)] = $t & 0x000000FF
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(detect_D_mem(reg_pre[rs]+(int)immediate,cyc))
				return -2;
			data_data[reg_pre[rs]+(int)immediate  ]=reg_pre[rt]     & 0x000000ff;
			return ++no_inst;
			break;
		case 0x0f : //lui $t = C << 16
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			reg_cur[rt]=immediate<<16;
			return ++no_inst;
			break;
		case 0x0c : //andi $t = $s & C(unsigned)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			reg_cur[rt]=reg_pre[rs] & immediate;
			return ++no_inst;
			break;
		case 0x0d : //ori $t = $s | C(unsigned)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			reg_cur[rt]=reg_pre[rs] | immediate;
			return ++no_inst;
			break;
		case 0x0e : //nori $t = ~($s | C(unsigned))
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			reg_cur[rt]=~(reg_pre[rs] | immediate);
			return ++no_inst;
			break;
		case 0x0a : //slti $t = ($s < C(signed) ), signed comparison
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			if(detect_error(1,cyc)) // error : write to register $0
				return++no_inst;
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			reg_cur[rt]= (reg_pre[rs]&0x80000000)<((int)(immediate&0x8000)<<16);
			return ++no_inst;
			break;
		case 0x04 : //beq if ($s == $t) go to PC+4+4*C(signed)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(reg_pre[rs]!=reg_pre[rt])
				return ++no_inst;
			else
			{
				pc_addr = inst_pc_addr[no_inst]+4+((int)immediate<<2);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
			}
			break;
		case 0x05 : //bne if ($s != $t) go to PC+4+4*C(signed)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(reg_pre[rs]==reg_pre[rt])
				return ++no_inst;
			else
			{
				pc_addr = inst_pc_addr[no_inst]+4+((int)immediate<<2);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
			}
			break;
		case 0x07 : //bgtz if ($s > 0) go to PC+4+4*C(signed)
			rs = (inst>>21)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(reg_pre[rs]>0)
				return ++no_inst;
			else
			{
				pc_addr = inst_pc_addr[no_inst]+4+((int)immediate<<2);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
			}
			break;
		case 0x3f : //halt halt the simulation
			return -2;
			break;
		default :
			return -1; // OH! error occur :(
			break;
    }
}
