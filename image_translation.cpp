#include <iostream>
#include <string>
#include "image_content.h"

unsigned int address; //26 bits
unsigned int immediate; //16 bits
unsigned int opcode, funct; //6 bits
unsigned int rt, rs, rd, shamt; //5 bits



//handle the inst
int trans_inst(unsigned int inst, int no_inst)
{
	unsigned int pc_addr; //a temp pc address to seek for the next inst :)
	
    opcode = inst>>26; // trans opcode in the beginning to determine the next step
    switch(opcode)
    {
        //R-type instructions op(6)_rs(5)_rt(5)_rd(5)_shamt(5)_funct(6)
        //opcode = 0x00
        case 0x00 :
            funct = inst%(1<<6);
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			rd = (inst>>11)%(1<<5);
            switch(funct)
            {
                case 0x20 : //add $d = $s + $t
					reg_cur[rd]=reg_pre[rs]+reg_pre[rt];
                    return no_inst++;
                case 0x21 : //addu $d = $s + $t(unsigned, no overflow exception)
					reg_cur[rd]=reg_pre[rs]+reg_pre[rt];
                    return no_inst++;
                case 0x22 : //sub $d = $s - $t
					reg_cur[rd]=reg_pre[rs]-reg_pre[rt];
                    return no_inst++;
                case 0x24 : //and $d = $s & $t
					reg_cur[rd]=reg_pre[rs]&reg_pre[rt];
                    return no_inst++;
                case 0x25 : //or $d = $s | $t
					reg_cur[rd]=reg_pre[rs]|reg_pre[rt];
                    return no_inst++;
				case 0x26 : //xor $d = $s ^ $t
					reg_cur[rd]=reg_pre[rs]^reg_pre[rt];
                    return no_inst++;
				case 0x27 : //nor $d = ~ ($s | $t)
					reg_cur[rd]=~(reg_pre[rs]|reg_pre[rt]);
                    return no_inst++;
				case 0x28 : //nand $d = ~($s & $t)
					reg_cur[rd]=~(reg_pre[rs]&reg_pre[rt]);
                    return no_inst++;
				case 0x2a : //slt $d = ($s < $t), signed comparison
					reg_cur[rd]=(reg_pre[rs]<reg_pre[rt]);
                    return no_inst++;
				case 0x00 : //sll $d = $t << C
					shamt = (inst>>6)%(1<<5);
					reg_cur[rd]= reg_pre[rt]<<shamt;
                    return no_inst++;
				case 0x02 : //srl $d = $t >> C
					shamt = (inst>>6)%(1<<5);
					reg_cur[rd]= reg_pre[rt]>>shamt;
                    return no_inst++;
				case 0x03 : //sra $d = $t >> C, with sign bit shifted in
					shamt = (inst>>6)%(1<<5);
					reg_cur[rd]= reg_pre[rt]>>shamt;
                    return no_inst++;
				case 0x08 : //jr PC=$s
					pc_addr=reg_pre[rs];
					for(unsigned int i = 0; i<no_inst_data; i++)
						if(pc_addr==inst_pc_addr[i])
							return no_inst=i;
				case 0x18 : //mult {Hi || Lo} = $s * $t
					{reg_cur[32]||reg_cur[33]} = reg_pre[rs] * reg_pre[rt];
                    return no_inst++;
				case 0x19 : //multu {Hi || Lo} = $s * $t (unsigned, no overflow exception)
					{reg_cur[32]||reg_cur[33]} = reg_pre[rs] * reg_pre[rt];
                    return no_inst++;
				case 0x10 : //mfhi $d = Hi
                    reg_cur[rd] = reg_pre[32];
					return no_inst++;
				case 0x12 : //mflo $d = Lo
                    reg_cur[rd] = reg_pre[33];
					return no_inst++;
				default :
					return -1; // error 
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
        case 0x03 : //jal
			address=inst%(1<<26);
			reg_cur[31]=(inst_pc_addr[no_inst]+4)>>27;
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
			reg_cur[rt] = reg_cur[rs] + immediate;
			return no_inst++;
			break;
		case 0x09 : //addiu $t = $s + C(unsigned, no overflow exception)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt] = reg_cur[rs] + immediate;
			return no_inst++;
			break;
		case 0x23 : //lw $t = 4 bytes from Memory[$s + C(signed)]
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt] = data_data[reg_pre[rs]+immediate];
			return no_inst++;
			break;
		case 0x21 : //lh $t = 2 bytes from Memory[$s + C(signed)], signed
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt] = data_data[reg_pre[rs]+immediate] & 0x0000ffff;
			return no_inst++;
			break;
		case 0x25 : //lhu $t = 2 bytes from Memory[$s + C(signed)], unsigned
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt] = data_data[reg_pre[rs]+immediate] & 0x0000ffff;
			return no_inst++;
			break;
		case 0x20 : //lb $t = Memory[$s + C(signed)], signed
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt] = data_data[reg_pre[rs]+immediate];
			return no_inst++;
			break;
		case 0x24 : //lbu $t = Memory[$s + C(signed)], unsigned
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt] = data_data[reg_pre[rs]+immediate];
			return no_inst++;
			break;
		case 0x2b : //sw 4 bytes from Memory[$s + C(signed)] = $t
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			data_data[reg_pre[rs]+immediate]=reg_pre[rt];
			return no_inst++;
			break;
		case 0x29 : //sh 2 bytes from Memory[$s + C(signed)] = $t & 0x0000FFFF
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			data_data[reg_pre[rs]+immediate]=reg_pre[rt] & 0x0000ffff;
			return no_inst++;
			break;
		case 0x28 : //sb Memory[$s + C(signed)] = $t & 0x000000FF
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			data_data[reg_pre[rs]+immediate]=reg_pre[rt] & 0x000000ff;
			return no_inst++;
			break;
		case 0x0f : //lui $t = C << 16
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=immediate<<16;
			return no_inst++;
			break;
		case 0x0c : //andi $t = $s & C(unsigned)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=reg_pre[rs] & immediate;
			return no_inst++;
			break;
		case 0x0d : //ori $t = $s | C(unsigned)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=reg_pre[rs] | immediate;
			return no_inst++;
			break;
		case 0x0e : //nori $t = ~($s | C(unsigned))
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]=~(reg_pre[rs] | immediate);
			return no_inst++;
			break;
		case 0x0a : //slti $t = ($s < C(signed) ), signed comparison
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			reg_cur[rt]= (reg_pre[rs]&0x80000000)<(immediate&0x8000)
			return no_inst++;
			break;
		case 0x04 : //beq if ($s == $t) go to PC+4+4*C(signed)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(reg_pre[rs]!=reg_pre[rt])
				return no_inst++;
			else
			{
				pc_addr = inst_pc_addr[no_inst]+4+(immediate<<2);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
			}
			break;
		case 0x05 : //bne if ($s != $t) go to PC+4+4*C(signed)
			rs = (inst>>21)%(1<<5);
			rt = (inst>>16)%(1<<5);
			immediate = inst%(1<<16);
			if(reg_pre[rs]==reg_pre[rt])
				return no_inst++;
			else
			{
				pc_addr = inst_pc_addr[no_inst]+4+(immediate<<2);
				for(unsigned int i = 0; i<no_inst_data; i++)
					if(pc_addr==inst_pc_addr[i])
						return no_inst=i;
			}
			break;
		case 0x07 : //bgtz if ($s > 0) go to PC+4+4*C(signed)
			rs = (inst>>21)%(1<<5);
			immediate = inst%(1<<16);
			if(reg_pre[rs]>0)
				return no_inst++;
			else
			{
				pc_addr = inst_pc_addr[no_inst]+4+(immediate<<2);
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