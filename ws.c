#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

typedef struct Inst_set {
	unsigned char* curbyte; 
	int format; 
	int num_reg;
	int is_mem;
} Inst_set;


void SicInstMnemonic(Inst_set *tmp, unsigned char curbyte){
	unsigned char opcode=curbyte>>2;
	opcode<<=2;
	switch(opcode) {
		//Format 1 Check
		case 0xC4:
            tmp->curbyte = "FIX";
            tmp->format = 1;
            tmp->is_mem = 0;
	}
    return;
}


int main() {
    Inst_set* instptr = malloc(sizeof(Inst_set));
    SicInstMnemonic(instptr, 0xC4);
    printf("%s", instptr->curbyte);
}



void SicInstMnemonic(Inst_set *tmp, unsigned char curbyte){
	unsigned char opcode=curbyte>>2;
	opcode<<=2;
	switch(opcode) {
		//Format 1 Check
		case 0xC4:
			tmp->curbyte = "FIX";
            tmp->format = 1;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
		case 0xC0:
			tmp->curbyte = "FLOAT";
            tmp->format = 1;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
		case 0xF4:
			tmp->curbyte = "HIO";
            tmp->format = 1;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
		case 0xC8:
			tmp->curbyte = "NORM";
            tmp->format = 1;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
		case 0xF0:
			tmp->curbyte = "SIO";
            tmp->format = 1;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
		case 0xF8:
			tmp->curbyte = "TIO";
            tmp->format = 1;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
			
		//Format 2 Check
		case 0x90:
			tmp->curbyte = "ADDR";
            tmp->format = 2;
            tmp->num_reg = 2;
			tmp->is_mem = 0;
		case 0xB4:
			tmp->curbyte = "CLEAR";
            tmp->format = 2;
            tmp->num_reg = 1;
			tmp->is_mem = 0;
		case 0xA0:
			tmp->curbyte = "COMPR";
            tmp->format = 2;
            tmp->num_reg = 2;
			tmp->is_mem = 0;
		case 0x9C:
			tmp->curbyte = "DIVR";
            tmp->format = 2;
            tmp->num_reg = 2;
			tmp->is_mem = 0;
		case 0x98:
			tmp->curbyte = "MULR";
            tmp->format = 2;
            tmp->num_reg = 2;
			tmp->is_mem = 0;
		case 0xAC:
			tmp->curbyte = "RMO";
            tmp->format = 2;
            tmp->num_reg = 2;
			tmp->is_mem = 0;
		case 0xA4:
			tmp->curbyte = "SHIFTL";
            tmp->format = 2;
            tmp->num_reg = 1;
			tmp->is_mem = 0;
		case 0xA8:
			tmp->curbyte = "SHIFTR";
            tmp->format = 2;
            tmp->num_reg = 1;
			tmp->is_mem = 0;
		case 0x94:
			tmp->curbyte = "SUBR";
            tmp->format = 2;
            tmp->num_reg = 2;
			tmp->is_mem = 0;
		case 0xB0:
			tmp->curbyte = "SVC";
            tmp->format = 2;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
		case 0xB8:
			tmp->curbyte = "TIXR";
            tmp->format = 2;
            tmp->num_reg = 1;
			tmp->is_mem = 0;
			
		//Format 0/3/4 Check
		case 0x18:
			tmp->curbyte = "ADD";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x58:
			tmp->curbyte = "ADDF";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x40:
			tmp->curbyte = "AND";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x28:
			tmp->curbyte = "COMP";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x88:
			tmp->curbyte = "COMPF";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x24:
			tmp->curbyte = "DIV";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x64:
			tmp->curbyte = "DIVF";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x3C:
			tmp->curbyte = "J";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x30:
			tmp->curbyte = "JEQ";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x34:
			tmp->curbyte = "JGT";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x38:
			tmp->curbyte = "JLT";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x48:
			tmp->curbyte = "JSUB";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x00:
			tmp->curbyte = "LDA";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x68:
			tmp->curbyte = "LDB";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x50:
			tmp->curbyte = "LDCH";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x70:
			tmp->curbyte = "LDF";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x08:
			tmp->curbyte = "LDL";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x6C:
			tmp->curbyte = "LDS";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x74:
			tmp->curbyte = "LDT";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x04:
			tmp->curbyte = "LDX";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0xD0:
			tmp->curbyte = "LPS";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x20:
			tmp->curbyte = "MUL";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x60:
			tmp->curbyte = "MULF";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x44:
			tmp->curbyte = "OR";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0xD8:
			tmp->curbyte = "RD";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x4C:
			tmp->curbyte = "RSUB";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 0;
		case 0xEC:
			tmp->curbyte = "SSK";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x0C:
			tmp->curbyte = "STA";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x78:
			tmp->curbyte = "STB";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x54:
			tmp->curbyte = "STCH";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x80:
			tmp->curbyte = "STF";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0xD4:
			tmp->curbyte = "STI";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x14:
			tmp->curbyte = "STL";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x7C:
			tmp->curbyte = "STS";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0xE8:
			tmp->curbyte = "STSW";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x84:
			tmp->curbyte = "STT";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x10:
			tmp->curbyte = "STX";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x1C:
			tmp->curbyte = "SUB";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x5C:
			tmp->curbyte = "SUBF";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0xE0:
			tmp->curbyte = "TD";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0x2C:
			tmp->curbyte = "TIX";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		case 0xDC:
			tmp->curbyte = "WD";
            tmp->format = 0;
            tmp->num_reg = 0;
			tmp->is_mem = 1;
		default:
			printf("No such opcode.\n");
			exit(1);
	}
}