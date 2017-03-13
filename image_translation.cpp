#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
//var repository
#include "image_content.h"
//error function 
#include "error_detect.h"

using namespace std;

//handle the inst
int trans_inst(unsigned int inst, int no_inst, int cyc)
{
	unsigned int mem_addr=0;
	unsigned int pc_addr=0; //a temp pc address to seek for the next inst :)
	long long int temp_rs, temp_rt;
    //cout << "instrution : " << hex << inst << endl;

    opcode = inst>>26; // trans opcode in the beginning to determine the next step
	if(opcode!=0x00)
		inst_pre=inst;
    cout << "opcode : " << hex << opcode << " cyc " << dec << (cyc-1) << endl;
    switch(opcode)
    {
        //R-type instructions op(6)_rs(5)_rt(5)_rd(5)_shamt(5)_funct(6)
        //opcode = 0x00
        case 0x00 :
            funct = inst%(1<<6);
			cout << "funct : " << hex << funct << dec << " cyc " << (cyc-1) << endl;
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			rd = (inst>>11)%(1<<5);
			if(funct!=0x18||funct!=0x19)
				inst_pre=inst;
            switch(funct)
            {
                case 0x20 : //add $d = $s + $t
					reg_cur[rd]=reg_pre[rs]+reg_pre[rt];
					detect_reg0(cyc); //detect write $0     //continue the sim
					detect_ovf(cyc);  //detect outcome ovf  //continue the sim
                    return ++no_inst;
					break;
                case 0x21 : //addu $d = $s + $t(unsigned, no overflow exception)
					reg_cur[rd]=(unsigned int)reg_pre[rs]+(unsigned int)reg_pre[rt];
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
                case 0x22 : //sub $d = $s - $t
					reg_cur[rd]=reg_pre[rs]-reg_pre[rt];
					cout << "cyc rs:"<< setw(8) << hex << reg_pre[rs] << endl;
					cout << "cyc rt:"<< setw(8) << hex << reg_pre[rt] << endl;
					detect_reg0(cyc); //detect write $0     //continue the sim
					detect_ovf(cyc);  //detect outcome ovf  //continue the sim
                    return ++no_inst;
					break;
                case 0x24 : //and $d = $s & $t
					reg_cur[rd]=reg_pre[rs]&reg_pre[rt];
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
                case 0x25 : //or $d = $s | $t
					reg_cur[rd]=reg_pre[rs]|reg_pre[rt];
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
				case 0x26 : //xor $d = $s ^ $t
					reg_cur[rd]=reg_pre[rs]^reg_pre[rt];
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
				case 0x27 : //nor $d = ~ ($s | $t)
					reg_cur[rd]=~(reg_pre[rs]|reg_pre[rt]);
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
				case 0x28 : //nand $d = ~($s & $t)
					reg_cur[rd]=~(reg_pre[rs]&reg_pre[rt]);
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
				case 0x2a : //slt $d = ($s < $t), signed comparison
					reg_cur[rd]=(reg_pre[rs]<reg_pre[rt]);
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
				case 0x00 : //sll $d = $t << C
					shamt = (inst>>6)%(1<<5);
					reg_cur[rd]= reg_pre[rt]<<shamt;
					if(inst!=0x00000000)  //NOP inst won't detect error 
						detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
				case 0x02 : //srl $d = $t >> C
					shamt = (inst>>6)%(1<<5);
					reg_cur[rd]= (unsigned int)reg_pre[rt]>>shamt;
					detect_reg0(cyc); //detect write $0     //continue the sim
                    return ++no_inst;
					break;
				case 0x03 : //sra $d = $t >> C, with sign bit shifted in
					shamt = (inst>>6)%(1<<5);
					if(reg_pre[rt]>=0)
						reg_cur[rd]= (reg_pre[rt]>>shamt);
					else
						reg_cur[rd]= ((0xffffffff00000000|(unsigned long long)reg_pre[rt])>>shamt);
					detect_reg0(cyc); //detect write $0     //continue the sim
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
					detect_ovr(cyc); //detect overwrite HI-LO //continue the sim
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
					detect_ovr(cyc); //detect overwrite HI-LO  //continue the sim
                    return ++no_inst;
					break;
				case 0x10 : //mfhi $d = Hi
					reg_cur[rd] = reg_pre[32];
					detect_reg0(cyc); //detect write $0     //continue the sim
					return ++no_inst;
					break;
				case 0x12 : //mflo $d = Lo
					reg_cur[rd] = reg_pre[33];
					detect_reg0(cyc); //detect write $0     //continue the sim
					return ++no_inst;
					break;
				default :
					cout << "illegal instruction found at 0x" << hex << setw(8) << setfill('0') << inst ;
					return -1; // error : illegal inst
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
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			reg_cur[rt] = reg_cur[rs] + (int)immediate;
			detect_reg0(cyc); //detect write $0     //continue the sim
			detect_ovf(cyc);  //detect outcome ovf  //continue the sim
			return ++no_inst;
			break;
		case 0x09 : //addiu $t = $s + C(unsigned, no overflow exception)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			reg_cur[rt] = reg_cur[rs] + immediate;
			detect_reg0(cyc); //detect write $0     //continue the sim
			return ++no_inst;
			break;
		case 0x23 : //lw $t = 4 bytes from Memory[$s + C(signed)]
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			mem_addr = (unsigned int)(reg_pre[rs]+(int)immediate+3);
			if(mem_addr<=1023)
				reg_cur[rt] =(data_data[reg_pre[rs]+(int)immediate]  <<24)
							+(data_data[reg_pre[rs]+(int)immediate+1]<<16)
							+(data_data[reg_pre[rs]+(int)immediate+2]<< 8)
							+(data_data[reg_pre[rs]+(int)immediate+3]    );
			detect_reg0(cyc); //detect write $0     //continue the sim
			detect_D_mem(reg_pre[rs]+(int)immediate+3,cyc); //detect the D mem  //halt the sim
			detect_misaligned(reg_pre[rs]+(int)immediate,cyc,4); //detect misaligned  //halt the sim
			return ++no_inst;
			break;
		case 0x21 : //lh $t = 2 bytes from Memory[$s + C(signed)], signed
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			mem_addr = (unsigned int)(reg_pre[rs]+(int)immediate+1);
			if(mem_addr<=1023)
			{
				reg_cur[rt] =(data_data[reg_pre[rs]+(int)immediate  ]<<8)
							+ data_data[reg_pre[rs]+(int)immediate+1];
				if(reg_cur[rt] & 0x00008000)
					reg_cur[rt] = reg_cur[rt] | 0xffff8000;
			}
			detect_reg0(cyc); //detect write $0     //continue the sim
			detect_D_mem(reg_pre[rs]+(int)immediate+1,cyc); //detect the D mem  //halt the sim
			detect_misaligned(reg_pre[rs]+(int)immediate,cyc,2); //detect misaligned  //halt the sim
			return ++no_inst;
			break;
		case 0x25 : //lhu $t = 2 bytes from Memory[$s + C(signed)], unsigned
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			mem_addr = (unsigned int)(reg_pre[rs]+(int)immediate+1);
			if(mem_addr<=1023)
				reg_cur[rt] =(data_data[reg_pre[rs]+(int)immediate]<<8)
							+ data_data[reg_pre[rs]+(int)immediate+1]  ;
			detect_reg0(cyc); //detect write $0     //continue the sim
			detect_D_mem(reg_pre[rs]+(int)immediate+1,cyc); //detect the D mem  //halt the sim
			detect_misaligned(reg_pre[rs]+(int)immediate,cyc,2); //detect misaligned  //halt the sim
			return ++no_inst;
			break;
		case 0x20 : //lb $t = Memory[$s + C(signed)], signed
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			mem_addr = (unsigned int)(reg_pre[rs]+(int)immediate);
			if(mem_addr<=1023)
			{
				reg_cur[rt] = data_data[reg_pre[rs]+(int)immediate];
				if(reg_cur[rt] & 0x0000080)
					reg_cur[rt] =reg_cur[rt] | 0xffffff00;
			}
			detect_reg0(cyc); //detect write $0     //continue the sim
			detect_D_mem(reg_pre[rs]+(int)immediate,cyc); //detect the D mem  //halt the sim
			return ++no_inst;
			break;
		case 0x24 : //lbu $t = Memory[$s + C(signed)], unsigned
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			mem_addr = (unsigned int)(reg_pre[rs]+(int)immediate);
			if(mem_addr<=1023)
				reg_cur[rt] = data_data[reg_pre[rs]+(int)immediate];
			detect_reg0(cyc); //detect write $0     //continue the sim
			detect_D_mem(reg_pre[rs]+(int)immediate,cyc); //detect the D mem  //halt the sim
			return ++no_inst;
			break;
		case 0x2b : //sw 4 bytes from Memory[$s + C(signed)] = $t
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			data_data[reg_pre[rs]+(int)immediate  ]=reg_pre[rt]>>24 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+1]=reg_pre[rt]>>16 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+2]=reg_pre[rt]>> 8 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+3]=reg_pre[rt]     & 0x000000ff;
			detect_D_mem(reg_pre[rs]+(int)immediate+3,cyc); //detect the D mem  //halt the sim
			detect_misaligned(reg_pre[rs]+(int)immediate,cyc,4); //detect misaligned  //halt the sim
			return ++no_inst;
			break;
		case 0x29 : //sh 2 bytes from Memory[$s + C(signed)] = $t & 0x0000FFFF
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			data_data[reg_pre[rs]+(int)immediate  ]=reg_pre[rt]>> 8 & 0x000000ff;
			data_data[reg_pre[rs]+(int)immediate+1]=reg_pre[rt]     & 0x000000ff;
			detect_D_mem(reg_pre[rs]+(int)immediate+1,cyc); //detect the D mem  //halt the sim
			detect_misaligned(reg_pre[rs]+(int)immediate,cyc,2); //detect misaligned  //halt the sim
			return ++no_inst;
			break;
		case 0x28 : //sb Memory[$s + C(signed)] = $t & 0x000000FF
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			data_data[reg_pre[rs]+(int)immediate  ]=reg_pre[rt]     & 0x000000ff;
			detect_D_mem(reg_pre[rs]+(int)immediate,cyc); //detect the D mem  //halt the sim
			return ++no_inst;
			break;
		case 0x0f : //lui $t = C << 16
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=immediate<<16;
			detect_reg0(cyc); //detect write $0     //continue the sim
			return ++no_inst;
			break;
		case 0x0c : //andi $t = $s & C(unsigned)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=reg_pre[rs] & immediate;
			detect_reg0(cyc); //detect write $0     //continue the sim
			return ++no_inst;
			break;
		case 0x0d : //ori $t = $s | C(unsigned)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=reg_pre[rs] | immediate;
			detect_reg0(cyc); //detect write $0     //continue the sim
			return ++no_inst;
			break;
		case 0x0e : //nori $t = ~($s | C(unsigned))
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=~(reg_pre[rs] | immediate);
			detect_reg0(cyc); //detect write $0     //continue the sim
			return ++no_inst;
			break;
		case 0x0a : //slti $t = ($s < C(signed) ), signed comparison
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			reg_cur[rt]= (reg_pre[rs])<((int)immediate);
			detect_reg0(cyc); //detect write $0     //continue the sim
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
				pc_addr = inst_pc_addr[no_inst]+4+((int)immediate*4);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
				if(pc_addr<init_pc) //check bounding edge (min)
					min_pc=min_pc<pc_addr?min_pc:pc_addr;
				if(pc_addr>init_max_pc) //check bounding edge (max)
					max_pc=max_pc>pc_addr?max_pc:pc_addr;
				if(max_pc-min_pc>1023) //detect error
					detect_D_mem(2048,cyc);
				else
				{
					reg_cur[34]=pc_addr;
					return 1000;
				}
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
				pc_addr = inst_pc_addr[no_inst]+4+((int)immediate*4);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
				if(pc_addr<init_pc) //check bounding edge (min)
					min_pc=min_pc<pc_addr?min_pc:pc_addr;
				if(pc_addr>init_max_pc) //check bounding edge (max)
					max_pc=max_pc>pc_addr?max_pc:pc_addr;
				if(max_pc-min_pc>1023) //detect error
					detect_D_mem(2048,cyc);
				else
				{
					reg_cur[34]=pc_addr;
					return 1000;
				}
			}
			break;
		case 0x07 : //bgtz if ($s > 0) go to PC+4+4*C(signed)
			rs = (inst>>21)%(1<<5);
			immediate = inst%(1<<16);
			if(immediate & 0x00008000)
				immediate = immediate | 0xffff0000;
			if(reg_cur[rs]<=0)
				return ++no_inst;
			else
			{
				pc_addr = inst_pc_addr[no_inst]+4+((int)immediate*4);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
				if(pc_addr<init_pc) //check bounding edge (min)
					min_pc=min_pc<pc_addr?min_pc:pc_addr;
				if(pc_addr>init_max_pc) //check bounding edge (max)
					max_pc=max_pc>pc_addr?max_pc:pc_addr;
				if(max_pc-min_pc>1023) //detect error
					detect_D_mem(2048,cyc);
				else
				{
					reg_cur[34]=pc_addr;
					return 1000;
				}
			}
			break;
		case 0x3f : //halt halt the simulation
			return -2;
			break;
		default :
			cout << "illegal instruction found at 0x" << hex << setw(8) << setfill('0') << inst ;
			return -1; // error : illegal inst
			break;
    }
}
