#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

int tcount;
int mcount;
int power_16[6] = {1, 16, 256, 4096, 65536, 1048576};
unsigned char names[100][6] = {"A1    ", "A2    ", "A3    ", "A4    ", "A5    ", "A6    ", "A7    ", "A8    ", "A9    ", "A10   ", "A11   ", "A12   ", "A13   ", "A14   ", "A15   ", "A16   ", "A17   ", "A18   ", "A19   ", "A20   ", "A21   ", "A22   ", "A23   ", "A24   ", "A25   ", "A26   ", "A27   ", "A28   ", "A29   ", "A30   ", "A31   ", "A32   ", "A33   ", "A34   ", "A35   ", "A36   ", "A37   ", "A38   ", "A39   ", "A40   ", "A41   ", "A42   ", "A43   ", "A44   ", "A45   ", "A46   ", "A47   ", "A48   ", "A49   ", "A50   ", "A51   ", "A52   ", "A53   ", "A54   ", "A55   ", "A56   ", "A57   ", "A58   ", "A59   ", "A60   ", "A61   ", "A62   ", "A63   ", "A64   ", "A65   ", "A66   ", "A67   ", "A68   ", "A69   ", "A70   ", "A71   ", "A72   ", "A73   ", "A74   ", "A75   ", "A76   ", "A77   ", "A78   ", "A79   ", "A80   ", "A81   ", "A82   ", "A83   ", "A84   ", "A85   ", "A86   ", "A87   ", "A88   ", "A89   ", "A90   ", "A91   ", "A92   ", "A93   ", "A94   ", "A95   ", "A96   ", "A97   ", "A98   ", "A99   ", "A100  "};
int na = 0;

enum boolean {false, true};

typedef struct Header {
	char name[7]; 				// Col.		2~7		Program name
	unsigned int start_addr;	// Col.		8~13	Starting address of object program (hex)
	unsigned int prg_length;	// Col.		14-19	Length of object program in bytes (hex)
} Header;

typedef struct Text {
	unsigned int start_addr;	// Col.		2~7     Starting address for object code in this record (hex)
	unsigned int rec_length;	// Col.		8~9     Length of object code in this record in bytes (hex)
	unsigned char inst[30];		// Col.		10~69   Object code, represented in hex (2 col. per byte)
} Text;

typedef struct Mod {
	unsigned int start_addr;	// Col.		2~7		Starting loc of the addr field to be modified, relative to the beginining of the program, in hex
	unsigned int mod_length;	// Col.		8~9		Length of the addr field to be modified, in half-bytes, in hex
} Mod;

typedef struct End {
	unsigned int first_inst;
} End;

typedef struct Instruction {
	unsigned int loc_ctr; 
	char label[7]; 
	char opname[8]; 
	char operand[11]; 
	unsigned char objcode[4]; 
	int format; 
} Instruction;

typedef struct node* link;
struct node {
	Instruction* instext_tmp;
	link next;
};

typedef struct Symbol {
	char label[7];
	unsigned int address;
	int value; /* made this signed as value is not necessarily an address*/
	char type;
} Symbol;

typedef struct symtab* symtab_link;
struct symtab {
	Symbol* sym;
	symtab_link next;
};

symtab_link symtab_head = NULL;

int				CharToNum				(unsigned char input, int pos, int size);
int				Bit						(unsigned char curbyte, int bytenx);
unsigned char	ByteToHalfByte			(unsigned char curbyte, int halfnx);

void			CountRecords			(FILE *objfile);
Header*			GetHeader				(FILE *objfile);
Text*			GetText					(FILE *objfile);
Mod*			GetMod					(FILE *objfile);
End*			GetEnd					(FILE *objfile);

link			FormatCall				(unsigned int reclength, unsigned char inst[30], link head, unsigned int locctr, int lastbyte);
Instruction*	Format1					(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]);
Instruction*	Format2					(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]);
Instruction*	Format0					(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]);
Instruction*	Format3					(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30], int basevalue);
Instruction*	Format4					(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]);			
Instruction*	ByteData				(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]);
link			InsertSTARTDirective	(link HEAD, char* progname, unsigned int LOCCTR);
link			InsertENDDirective		(link HEAD, unsigned int LOCCTR);
link			InsertBASEDirective		(link HEAD, unsigned int LOCCTR, unsigned int targetaddress);
Instruction*	OpcodeCopy				(unsigned char curbyte, Instruction* instptr);
link			InsertRESDirectives		(link HEAD, unsigned int LOCCTR, int nextaddr);

char*			GetSymbolName			(unsigned int value);
int				SicInstCheck			(unsigned char curbyte);
char*			SicInstMnemonic			(unsigned char curbyte);
char* 			RegisterTab				(unsigned int regnum);
int 			NumOfRegisters			(unsigned char curbyte);
unsigned int	NextSymbolAddress		(unsigned int value);


link 			Add						(link head, Instruction *inst);
unsigned int	GetLocctr				(link head);
void			PrintList				(link head);
void			PrintSym				(symtab_link head);
void			WriteSym				(symtab_link head, char *fileName);
void			WriteFile				(link head, char *fileName, int fileflag);
void			FreeItems				(link head);
void			FreeList				(link head);





int main(int argc, const char * argv[]) {
	char ch;
	int textnx = 0;
	int modnx = 0;

	if(argv[1]==NULL) {
		printf("input: \"sample2.obj\" \n");
		exit(1);
	}

	char *file_name;
	file_name   = (char*)argv[1];
	FILE *obj_file;
	obj_file    = fopen(file_name, "rb");

	CountRecords(obj_file);

	Text	**Txt_rec	= malloc(sizeof(Text*) * tcount);
	Mod		**Mod_rec	= malloc(sizeof(Mod*)  * mcount);

	Header	*Head_rec	= GetHeader(obj_file);
	while((ch = fgetc(obj_file)) != 'E') {
		if		(ch == 'T') {
			Txt_rec[textnx++]	= GetText(obj_file);
		} 
		else if	(ch == 'M') {
			Mod_rec[modnx++]	= GetMod(obj_file);
		} 
	}
	End		*End_rec	= GetEnd(obj_file);

	// printf("%d\n\n", Txt_rec[0]->rec_length);
	// printf("%d\n\n", Txt_rec[1]->rec_length);

	int LOCCTR;
	LOCCTR = 0;
	int nextaddr = 0; /* address of next instruction */
	int lastbyte = Head_rec->start_addr + Head_rec->prg_length;
	link head = NULL;
	
	head = InsertSTARTDirective(head, Head_rec->name, Head_rec->start_addr);
	for(textnx = 0; textnx < tcount; textnx++) {
		LOCCTR = Txt_rec[textnx]->start_addr;
		if(textnx + 1 < tcount)
			nextaddr = Txt_rec[textnx + 1]->start_addr;
		else
			nextaddr = lastbyte; /* EOF case */
		head = FormatCall(Txt_rec[textnx]->rec_length, Txt_rec[textnx]->inst, head, LOCCTR, lastbyte);
		LOCCTR += Txt_rec[textnx]->rec_length;
		head = InsertRESDirectives(head, LOCCTR, nextaddr);
	}
	head = InsertENDDirective(head, Head_rec->start_addr);

	/*********** STEP 3 ***********/
	printf("Program name: %s\n", Head_rec->name);
	printf("Starting address: %06X\n", Head_rec->start_addr);
	printf("Program length: %06X\n\n", Head_rec->prg_length);
	printf("\nSOURCE CODE DISASSEMBLEMENT\n");
	PrintList(head);
	PrintSym(symtab_head);
	WriteSym(symtab_head, "output_symtab.sym");
	// WriteFile(head, sic_name, 0);
	WriteFile(head, "output_symtab.lis", 1);
	/******** END OF STEP 3 *******/
	
	// printf("\nFiles %s and %s were created.\n\n", sic_name, lis_name);
	
	// free(obj_name); free(sym_name);
	// free(sic_name); free(lis_name);
	for(int i = 0; i < tcount; i++)
		free(Txt_rec[i]);
	for(int i = 0; i < mcount; i++)
		free(Mod_rec[i]);
	free(Txt_rec); free(Mod_rec); free(Head_rec); free(End_rec);
	FreeItems(head);
	FreeList(head);

	return 0;
}


int CharToNum(unsigned char input, int pos, int size) {
	unsigned int charnum = (unsigned int)input;
	if		(charnum >= 48 && charnum <= 57) { // [0-9]
		charnum -= 48;
	}
	else if	(charnum >= 65 && charnum <= 70) { // [A-F]
		charnum -= 55;
	}
	charnum = charnum * power_16[size - pos - 1];
	return charnum;
}

int Bit(unsigned char curbyte, int bytenx) {
	curbyte >>= bytenx;
	int bitval = curbyte & 1;
	return bitval;
}

unsigned char ByteToHalfByte(unsigned char curbyte, int halfnx) {
	unsigned char value;
	if(halfnx == 1)
		return value = (Bit(curbyte,7)*8 + Bit(curbyte,6)*4 + Bit(curbyte,5)*2 + Bit(curbyte,4));
	else
		return value = (Bit(curbyte,3)*8 + Bit(curbyte,2)*4 + Bit(curbyte,1)*2 + Bit(curbyte,0));
}

void CountRecords(FILE *objfile) {
	tcount = 0; 
	mcount = 0;
	char ch;
	for (int i = 0; i < 19; i++)
		fgetc(objfile); 
	while ( (ch = fgetc(objfile)) != EOF ) {
		if		(ch == 'T') tcount++;
		else if	(ch == 'M') mcount++;
	}
	rewind(objfile);
}


Header* GetHeader(FILE *objfile) {
	Header *head_tmp = malloc(sizeof(Header));
	head_tmp->start_addr = 0;
	head_tmp->prg_length = 0;
	unsigned char ch;
	int namenx = 0, addrnx = 0, lennx = 0;
	ch = fgetc(objfile);
	for(int filenx = 1; filenx < 19; filenx++) {
		ch = fgetc(objfile);
		if		(filenx < 7 ) {
			head_tmp->name[namenx++] = ch;
		}
		else if	(filenx < 13) {
			head_tmp->start_addr += CharToNum(ch, addrnx++, 6);
		}
		else {
			head_tmp->prg_length += CharToNum(ch, lennx++, 6);
		}
	}
	head_tmp->name[6] = '\0';
	fgetc(objfile); 
	return head_tmp;
}

Text* GetText(FILE *objfile) {
	Text *text_tmp = malloc(sizeof(Text));
	text_tmp->start_addr = 0;
	text_tmp->rec_length = 0;
	int adrnx = 0, lennx = 0, instnx = 0;
	unsigned char ch;
	for(int filenx = 0; filenx < 8; filenx++) {
		ch= fgetc(objfile);
		if (filenx < 6) {
			text_tmp->start_addr += CharToNum(ch, adrnx++, 6);
		}
		else {
			text_tmp->rec_length += CharToNum(ch, lennx++, 2);
		}
	}
	for(instnx = 0; instnx < text_tmp->rec_length; instnx++) {
		text_tmp->inst[instnx] = 0;
		ch = fgetc(objfile);
		text_tmp->inst[instnx] += CharToNum(ch, 0, 2);
		ch = fgetc(objfile);
		text_tmp->inst[instnx] += CharToNum(ch, 1, 2);
	}
	return text_tmp;
} 

Mod *GetMod(FILE *objfile) {
	Mod *mod_tmp = malloc(sizeof(Mod));
	mod_tmp->start_addr = 0;
	mod_tmp->mod_length = 0;
	int adrnx = 0, lennx = 0;
	unsigned char ch;
	for(int filenx = 0; filenx < 8; filenx++) {
		ch = fgetc(objfile);
		if		(filenx < 6 ) {
			mod_tmp->start_addr += CharToNum(ch, adrnx++, 6);
		}
		else {
			mod_tmp->mod_length += CharToNum(ch, lennx++, 2);
		}
	}
	return mod_tmp;
} 

End *GetEnd(FILE *objfile) {
	End *end_tmp = malloc(sizeof(End));
	end_tmp->first_inst = 0;
	int adrnx = 0;
	char ch;
	for (int filenx = 0; filenx < 6; filenx++) {
		ch = fgetc(objfile);
		end_tmp->first_inst += CharToNum(ch, adrnx++, 6);
	}
	return end_tmp;
} 


/* basevalue is kept track of throughout all of the  Text records. */
unsigned int basevalue = 0; 

link FormatCall(unsigned int reclength, unsigned char inst[30], link head, unsigned int locctr, int lastbyte) {
    for(int i = 0; i < reclength; i++) {
		Instruction* instptr = malloc(sizeof(Instruction));
        
        instptr->loc_ctr = 0;
        strncpy(instptr->operand, "         \0", 10);
		
        unsigned char curbyte = inst[i];

		strncpy(instptr->label, "      \0", 7);

		int format = SicInstCheck(curbyte);

		if(format==1) {
			instptr = Format1(instptr, curbyte, locctr, i, inst);
			locctr++;
		}
		else if((i+1>=reclength) || (format == 5)) {
			instptr = ByteData(instptr, curbyte, locctr, i, inst);
			locctr++;
		}
		else if(format==2) {
			instptr = Format2(instptr, curbyte, locctr, i, inst);
			locctr+=2;
			i++;
		}
		// sic only, ni = 00
		else if( Bit(curbyte,1)==0 && Bit(curbyte,0)==0 ) {
			instptr = Format0(instptr, curbyte, locctr, i, inst);
			locctr+=3;
			i+=2;
		}
		// sic/xe
		else if(Bit(inst[i+1],4)==0) {
			instptr = Format3(instptr, curbyte, locctr, i, inst, basevalue);
			locctr+=3;
			i+=2;
		}
		else {
			instptr = Format4(instptr, curbyte, locctr, i, inst);
			locctr+=4;
			i+=3;
		}

		head = Add(head, instptr);

		if(strcmp(SicInstMnemonic(curbyte),"LDB")==0) {
			int disp1 = ByteToHalfByte(instptr->objcode[1], 0) << 8;
			int disp2 = instptr->objcode[2];
			int disp = disp1 + disp2;
			unsigned int targetaddress = disp + locctr;
			head = InsertBASEDirective(head, locctr-3, targetaddress);
			basevalue = targetaddress;
		}
    }
	return head;
}


Instruction* Format1(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]) {
	instptr->loc_ctr = locctr;
	instptr->opname[0]=' ';
	instptr = OpcodeCopy(curbyte, instptr);
	instptr->format=1;
	instptr->objcode[0] = inst[i];
	return instptr;
}
Instruction* Format2(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]) {
	instptr->loc_ctr=locctr;
	instptr->opname[0]=' ';
	instptr = OpcodeCopy(curbyte, instptr);
	unsigned char r1 = ByteToHalfByte(inst[i+1], 1);
	unsigned char r2 = ByteToHalfByte(inst[i+1], 0);
	strncpy(instptr->operand+1, RegisterTab(r1), 2);
	if(NumOfRegisters(curbyte)==1) {
		strncpy(instptr->operand+1+strlen(RegisterTab(r1)), ",", 1);
		strncpy(instptr->operand+2+strlen(RegisterTab(r1)), RegisterTab(r2), 2);
	}
	for(int j = 2; j < 8; j++) {
		if(instptr->operand[j]<44)
			instptr->operand[j]=' ';
	}
	instptr->format=2;
	instptr->objcode[0] = inst[i];
	instptr->objcode[1] = inst[i+1];
	return instptr;
}
// sic inst
Instruction* Format0(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]) {
	instptr->loc_ctr=locctr;
	instptr->opname[0]=' ';
	instptr = OpcodeCopy(curbyte, instptr);
	unsigned int disp1 = (Bit(inst[i+1],6)<<14) + (Bit(inst[i+1],5)<<13) + (Bit(inst[i+1],4)<<12);
	unsigned int disp2 = ByteToHalfByte(inst[i+1], 0) << 8;
	unsigned char disp3 = inst[i+2];
	enum boolean xflag=false;
	if(Bit(inst[i+1],7)==1)
		xflag=true;
	unsigned int targetaddress = disp1 + disp2 + disp3;

	if (curbyte != 0x4C) {
		strncpy(instptr->operand+1, GetSymbolName(targetaddress), 7);
		if(xflag==true) {
			strncpy(instptr->operand+ (strlen(GetSymbolName(targetaddress))+1), ",X", 2);
		}
		else {
			strncpy(instptr->operand+ (strlen(GetSymbolName(targetaddress))+1), "  ", 2);
		}
	}
	else {
		strncpy(instptr->operand+1, "        ", 9);
	}
	instptr->format=3;
	instptr->objcode[0] = inst[i];
	instptr->objcode[1] = inst[i+1];
	instptr->objcode[2] = inst[i+2];
	return instptr;
}


// sic/ex inst
Instruction* Format3(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30], int basevalue) {
	unsigned int targetaddress;
	instptr->loc_ctr=locctr;
	instptr->opname[0]=' ';
	instptr = OpcodeCopy(curbyte, instptr);
	int disp1 = ByteToHalfByte(inst[i+1], 0) << 8;
	int disp2 = inst[i+2];
	int disp = disp1 + disp2;

	/*Makes disp negative*/
	if(Bit(ByteToHalfByte(inst[i+1],0),3)==1)
		disp=-(0x1000-disp);

	enum boolean xflag=false;
	char* addressingmode;
	int code=0;

	/*Indexed addressing check*/
	if(Bit(inst[i+1],7)==1)
		xflag=true;
		
	/*Indirect or Immediate check*/
	if(Bit(curbyte,1)==1&&Bit(curbyte,0)==0) {
		//addressingmode = "Indirect";
		instptr->operand[0]='@';
	}
	else if(Bit(curbyte,1)==0&&Bit(curbyte,0)==1) {
		//addressingmode = "Immediate";
		instptr->operand[0]='#';
		code = 1;
		sprintf(instptr->operand+1, "%d         ", disp);
	}
	else {
		instptr->operand[0]=' ';
	}
	
	/*Addressing mode check*/
	if(Bit(inst[i+1],5)==1) {
		addressingmode = "PC Relative";
		targetaddress = disp + locctr + 3;
	}
	else if(Bit(inst[i+1],6)==1) {
		addressingmode = "Base Relative";
		targetaddress = disp + basevalue;
	}
	else {
		addressingmode = "Direct";
		targetaddress = disp;
	}

	unsigned char opcode=curbyte>>2; opcode<<=2;

	if ( ((code == 0) && (opcode != 0x4C)) || ((code == 1) && (opcode == 0x68)) )  {
		strncpy(instptr->operand+1, GetSymbolName(targetaddress), 7);
		if(xflag==true) {
			strncpy(instptr->operand+ (strlen(GetSymbolName(targetaddress))+1), ",X", 2);
		}
		else {
			strncpy(instptr->operand+ (strlen(GetSymbolName(targetaddress))+1), "  ", 2);
		}
	}

	instptr->format=3;
	instptr->objcode[0] = inst[i];
	instptr->objcode[1] = inst[i+1];
	instptr->objcode[2] = inst[i+2];
	return instptr;
}

Instruction* Format4(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]) {
	instptr->loc_ctr=locctr;
	instptr->opname[0]='+';
	instptr = OpcodeCopy(curbyte, instptr);
	unsigned int address1 = ByteToHalfByte(inst[i+1], 0) << 16;
	unsigned int address2 = inst[i+2] << 8;
	unsigned char address3 = inst[i+3];
	unsigned int address = address1 + address2 + address3;
	enum boolean xflag=false;
	char* addressingmode;

	int code = 0;

	if(Bit(inst[i+1],7)==1)
		xflag=true;

	if(Bit(curbyte,1)==1&&Bit(curbyte,0)==0) {
		addressingmode = "Indirect";
		instptr->operand[0]='@';
	}
	else if(Bit(curbyte,1)==0&&Bit(curbyte,0)==1) {
		addressingmode = "Immediate";
		instptr->operand[0]='#';
		code = 1;
		sprintf(instptr->operand+1, "%d    ", address);
	}
	else {
		instptr->operand[0]=' ';
		addressingmode = "Direct";
	}

	unsigned char opcode=curbyte>>2; opcode<<=2;

	
	if ((code == 0) && (opcode != 0x4C)) {
		strncpy(instptr->operand+1, GetSymbolName(address), 7);
		if(xflag==true) {
			strncpy(instptr->operand+ (strlen(GetSymbolName(address))+1), ",X", 2);
		}
		else {
			strncpy(instptr->operand+ (strlen(GetSymbolName(address))+1), "  ", 2);
		}
	}

	instptr->format=4;
	instptr->objcode[0] = inst[i];
	instptr->objcode[1] = inst[i+1];
	instptr->objcode[2] = inst[i+2];
	instptr->objcode[3] = inst[i+3];
	return instptr;
}

Instruction* ByteData(Instruction* instptr, unsigned char curbyte, unsigned int locctr, int i, unsigned char inst[30]) {
	instptr->loc_ctr=locctr;
	instptr->format = 1;//indicates number of objcode bytes to produce
	strncpy(instptr->opname, "BYTE  \0", 8);
	instptr->objcode[0] = inst[i];
	instptr->operand[0]=' ';
	strncpy(instptr->operand+1, "X'",2);
	sprintf(instptr->operand+3, "%02d", curbyte);
	strncpy(instptr->operand+5, "'    ",5);
	return instptr;
}

link InsertRESDirectives(link HEAD, unsigned int LOCCTR, int nextaddr) {
    while(LOCCTR < nextaddr) {
        int size = 0, nextsymaddr = 0;
        Instruction *RES = malloc(sizeof(Instruction));
        RES->format = -1;
        RES->objcode[0] = ' ';
        RES->objcode[1] = ' ';
        RES->objcode[2] = ' ';
        RES->objcode[3] = '\0';
        RES->loc_ctr = LOCCTR;

        strncpy(RES->label, GetSymbolName(LOCCTR), 7);
        nextsymaddr = NextSymbolAddress(LOCCTR);

        if(nextaddr > nextsymaddr && nextsymaddr != -1) {
            size = nextsymaddr - LOCCTR;
            LOCCTR = nextsymaddr;
        } else {
            size = nextaddr - LOCCTR;
            LOCCTR = nextaddr;
        }
        if(size % 3 == 0) {
            strncpy(RES->opname, " RESW  \0", 8);
            sprintf(RES->operand, " %d", size/3);
        } else {
            strncpy(RES->opname, " RESB  \0", 8);
            sprintf(RES->operand, " %d", size);
        }
        HEAD = Add(HEAD, RES);
    }
    return HEAD;
}

link InsertSTARTDirective(link HEAD, char* progname, unsigned int LOCCTR) {
    Instruction *START = malloc(sizeof(Instruction));
    START->format = -1;
    START->objcode[0] = ' ';
    START->objcode[1] = ' ';
	START->objcode[2] = ' ';
	START->objcode[3] = '\0';
    START->loc_ctr = LOCCTR;
	strncpy(START->opname, " START \0", 8);
	sprintf(START->operand, " %X", LOCCTR);
    strncpy(START->label, progname, 7);
    HEAD = Add(HEAD, START);
    return HEAD;
}

link InsertENDDirective(link HEAD, unsigned int LOCCTR) {
    Instruction *END = malloc(sizeof(Instruction));
    END->format = -2;
    END->objcode[0] = ' ';
    END->objcode[1] = ' ';
    END->objcode[2] = ' ';
    END->objcode[3] = '\0';
	strncpy(END->opname, " END   \0", 8);
    strncpy(END->label, "      \0", 7);
	strncpy(END->operand, GetSymbolName(LOCCTR), 9);
    HEAD = Add(HEAD, END);
    return HEAD;
} 


link InsertBASEDirective(link HEAD, unsigned int LOCCTR, unsigned int targetaddress) {
	Instruction *BASE = malloc(sizeof(Instruction));
    BASE->format = -2;
    BASE->objcode[0] = ' ';
    BASE->objcode[1] = ' ';
    BASE->objcode[2] = ' ';
    BASE->objcode[3] = '\0';
	BASE->loc_ctr=LOCCTR;
	strncpy(BASE->opname, "BASE  \0",8);
    strncpy(BASE->label, "      \0", 7);
	strncpy(BASE->operand, GetSymbolName(targetaddress), 9);
    HEAD = Add(HEAD, BASE);
    return HEAD;
}


Instruction* OpcodeCopy(unsigned char curbyte, Instruction* instptr) {
	char* opcode = SicInstMnemonic(curbyte);
	int i;
    for(i=1;i<strlen(opcode)+1;i++)
		instptr->opname[i] = opcode[i-1];
	while(i!=7) {
		instptr->opname[i] = ' ';
		i++;
	}
	instptr->opname[7]='\0';
	return instptr;
}


link Add(link head, Instruction *inst) {
	link listptr = head;
	link newbie = malloc(sizeof(*newbie));
	newbie->instext_tmp = malloc(sizeof(Instruction*));
	newbie->instext_tmp = inst;
	newbie->next=NULL;
	if(head==NULL)
		head = newbie;
	else {
		while(listptr->next!=NULL) {
			listptr = listptr->next;
		}
		listptr->next=newbie;
	}
    return head;
}

char *GetSymbolName(unsigned int value) {
    char *label = malloc(7);
    strncpy(label, "      \0", 7);
	symtab_link now = symtab_head;
    while(now != NULL) {
        if(value == now->sym->address) {
            strncpy(label, now->sym->label, 7);
            return label;
        }
        now = now->next;
    }
	Symbol *tmp = malloc(sizeof(*tmp));
	tmp->address = value;
	strncpy(tmp->label, names[na++], 6);
	tmp->type = ' ';
	tmp->value = value;
	symtab_link new = malloc(sizeof(*new));
	new->sym = malloc(sizeof(Symbol*));
	new->sym = tmp;
	new->next = symtab_head;
	symtab_head = new;
    return new->sym->label;
} 

unsigned int NextSymbolAddress(unsigned int value) {
	symtab_link now = symtab_head;
	symtab_link tmp = NULL;
	unsigned int addr = -1;
    while(now != NULL) {
        if ((now->sym->address > value) && (now->sym->address - value < addr) ) {
            addr = now->sym->address - value;
			tmp = now;
        }
		now = now->next;
    }
	if (tmp == NULL) return -1;
	else return tmp->sym->address;
}


int SicInstCheck(unsigned char curbyte){
	unsigned char opcode=curbyte>>2;
	opcode<<=2;

	switch(opcode) {
		//Format 1 Check
		case 0xC4:
		case 0xC0:
		case 0xF4:
		case 0xC8:
		case 0xF0:
 		case 0xF8:
			if (curbyte == opcode) {
				return 1;	
			}
			else {
				return 5;
			}
		//Format 2 Check
		case 0x90:
		case 0xB4:
		case 0xA0:
		case 0x9C:
		case 0x98:
		case 0xAC:
		case 0xA4:
		case 0xA8:
		case 0x94:
		case 0xB0:
		case 0xB8:
			return 2;
			
		//Format 0/3/4 Check
		default:
			return 0;
	}
}


char* SicInstMnemonic(unsigned char curbyte){
	unsigned char opcode=curbyte>>2;
	opcode<<=2;
	// printf("SicInstMnemonic, curbyte : %#X\n", curbyte);
	// printf("SicInstMnemonic, opcode : %#X\n", opcode);
	switch(opcode) {
		//Format 1 Check
		case 0xC4:
			return "FIX";
		case 0xC0:
			return "FLOAT";
		case 0xF4:
			return "HIO";
		case 0xC8:
			return "NORM";
		case 0xF0:
			return "SIO";
		case 0xF8:
			return "TIO";
			
		//Format 2 Check
		case 0x90:
			return "ADDR";
		case 0xB4:
			return "CLEAR";
		case 0xA0:
			return "COMPR";
		case 0x9C:
			return "DIVR";
		case 0x98:
			return "MULR";
		case 0xAC:
			return "RMO";
		case 0xA4:
			return "SHIFTL";
		case 0xA8:
			return "SHIFTR";
		case 0x94:
			return "SUBR";
		case 0xB0:
			return "SVC";
		case 0xB8:
			return "TIXR";
			
		//Format 0/3/4 Check
		case 0x18:
			return "ADD";
		case 0x58:
			return "ADDF";
		case 0x40:
			return "AND";
		case 0x28:
			return "COMP";
		case 0x88:
			return "COMPF";
		case 0x24:
			return "DIV";
		case 0x64:
			return "DIVF";
		case 0x3C:
			return "J";
		case 0x30:
			return "JEQ";
		case 0x34:
			return "JGT";
		case 0x38:
			return "JLT";
		case 0x48:
			return "JSUB";
		case 0x00:
			return "LDA";
		case 0x68:
			return "LDB";
		case 0x50:
			return "LDCH";
		case 0x70:
			return "LDF";
		case 0x08:
			return "LDL";
		case 0x6C:
			return "LDS";
		case 0x74:
			return "LDT";
		case 0x04:
			return "LDX";
		case 0xD0:
			return "LPS";
		case 0x20:
			return "MUL";
		case 0x60:
			return "MULF";
		case 0x44:
			return "OR";
		case 0xD8:
			return "RD";
		case 0x4C:
			return "RSUB";
		case 0xEC:
			return "SSK";
		case 0x0C:
			return "STA";
		case 0x78:
			return "STB";
		case 0x54:
			return "STCH";
		case 0x80:
			return "STF";
		case 0xD4:
			return "STI";
		case 0x14:
			return "STL";
		case 0x7C:
			return "STS";
		case 0xE8:
			return "STSW";
		case 0x84:
			return "STT";
		case 0x10:
			return "STX";
		case 0x1C:
			return "SUB";
		case 0x5C:
			return "SUBF";
		case 0xE0:
			return "TD";
		case 0x2C:
			return "TIX";
		case 0xDC:
			return "WD";
		default:
			printf("No such opcode.\n");
			exit(1);
	}
}

 
char* RegisterTab(unsigned int regnum) {
	char* regtab[10] = {"A", "X", "L", "B", "S", "T", "F", " ", "PC", "SW"};
	return regtab[regnum];
}
 
int NumOfRegisters(unsigned char curbyte) {
	unsigned char opcode=curbyte>>2;
	opcode<<=2;
	switch(opcode) {
		case 0xB4:    /*1 register*/
		case 0xB8:
			return 0;
		default:      /*2 registers*/
			return 1;
	}
}


unsigned int GetLocctr(link head) {
    link listptr = head;
	if(head==NULL) {
		return 0;
	}
	while(listptr->next!=NULL) {
		listptr = listptr->next;
	}
	if(listptr->instext_tmp->format==1)
		return listptr->instext_tmp->loc_ctr+1;
	else if(listptr->instext_tmp->format==2)
		return listptr->instext_tmp->loc_ctr+2;
	else if(listptr->instext_tmp->format==4)
		return listptr->instext_tmp->loc_ctr+4;
	else
		return listptr->instext_tmp->loc_ctr+3;
} 


void PrintSym(symtab_link head) {
	symtab_link symptr = head;
	printf("%6s ", "Label");
	printf("%4s  ", "ADDR");
	printf("\n");
	while(symptr!=NULL) {
		printf("%6s ", symptr->sym->label);
		printf("%04X  ", symptr->sym->address);
		symptr = symptr->next;
		printf("\n");
	}
	return;
}

void WriteSym(symtab_link head, char *fileName) {
	FILE *fPtr = fopen(fileName, "wb");
	symtab_link symptr = head;
	fprintf(fPtr, "%6s ", "Label");
	fprintf(fPtr, "%4s  ", "ADDR");
	fprintf(fPtr, "\n");
	while(symptr!=NULL) {
		fprintf(fPtr, "%6s ", symptr->sym->label);
		fprintf(fPtr, "%04X  ", symptr->sym->address);
		symptr = symptr->next;
		fprintf(fPtr, "\n");
	}
	return;
}

void PrintList(link head) {
	link listptr = head;
	int cnt = 0;
	while(listptr!=NULL) {
		if(listptr->instext_tmp->format!=-2) {
			printf("%04X  ", listptr->instext_tmp->loc_ctr);
		}
		else {
			printf("      ");
		}
		// search label
		if ( (listptr->next != NULL) && ( cnt != 0 ) ) {
			symtab_link now = symtab_head;
			while(now != NULL) {
				if(listptr->instext_tmp->loc_ctr == now->sym->address) {
					strncpy(listptr->instext_tmp->label, now->sym->label, 7);
				}
				now = now->next;
			}
		}

		printf("%6s ", listptr->instext_tmp->label);

        printf("%7s  ", listptr->instext_tmp->opname);
        
        listptr->instext_tmp->operand[9] = '\0';
		printf("%7s   ", listptr->instext_tmp->operand);
        
        int format = listptr->instext_tmp->format;
        int objlen = 0;
        if(format == 0 || format == 3) {
            objlen = 3;
        } 
		else if (format == 4) {
            objlen = 4;
        } 
		else if (format == 2) {
            objlen = 2;
        } 
		else if (format == 1 || format == 5) {
            objlen = 1;
        }
            
        for(int j = 0; j < objlen; j++) {
            printf("%02X", listptr->instext_tmp->objcode[j]);
        }
        
		printf("\n");
		listptr=listptr->next;
		cnt++;
	}
    
	return;
}

void WriteFile(link head, char *fileName, int fileflag) {
	FILE *fPtr = fopen(fileName, "wb");
	link listptr = head;
	int cnt = 0;
	while(listptr!=NULL) {
		if(fileflag==1) {
			if(listptr->instext_tmp->format!=-2) {
				fprintf(fPtr, "%04X  ", listptr->instext_tmp->loc_ctr);
			}
			else {
				fprintf(fPtr, "      ");
			}
		}

		fprintf(fPtr, "%6s ", listptr->instext_tmp->label);

        fprintf(fPtr, "%7s  ", listptr->instext_tmp->opname);
        
        // operand[0] is unitialized often
        listptr->instext_tmp->operand[9] = '\0';
		fprintf(fPtr, "%7s   ", listptr->instext_tmp->operand);
        
        if(fileflag==1) {
			int format = listptr->instext_tmp->format;
			int objlen = 0;
			if(format == 0 || format == 3) {
				objlen = 3;
			} 
			else if (format == 4) {
				objlen = 4;
			} 
			else if (format == 2) {
				objlen = 2;
			} 
			else if (format == 1 || format == 5) {
				objlen = 1;
			}
				
			for(int j = 0; j < objlen; j++) {
				fprintf(fPtr, "%02X", listptr->instext_tmp->objcode[j]);
			}
		}
        fprintf(fPtr, "\n");
		listptr=listptr->next;
		cnt++;
	}
    fclose(fPtr);
	return;
}

void FreeItems(link head) {
	link listPtr = head;
	while(listPtr!=NULL){
		free(listPtr->instext_tmp);
		listPtr=listPtr->next;
	}
	return;
}

void FreeList(link head) {
	link listPtr;
	while(head!=NULL){
		listPtr = head;
		head = head->next;
		free(listPtr);
	}
	return;
}

