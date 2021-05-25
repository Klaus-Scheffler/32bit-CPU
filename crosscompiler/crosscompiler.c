#include <stdio.h>
#include <stdlib.h>

#define regisgteroffset 0x00100000   // location of registers %0-%15,%BP,%SP  = registeroffset + 0...17
#define globalvaloffset 0x00500000   // location of global variables
#define inarg1		    0x00200000   // location of help registers for MUL, DIV
#define inarg2		    0x00200001   // location of help registers for MUL, DIV
#define outarg1		    0x00200002   // location of help registers for MUL, DIV
#define outarg2		    0x00200003   // location of help registers for MUL, DIV

#define MULadr		    0x00400000   // location MUL subroutine
#define DIVadr		    0x00400000   // location DIV subroutine

#define NOP  0x00000000
#define JEQ  0x01000000
#define JNE  0x02000000
#define JLE  0x03000000
#define JGE  0x04000000
#define JLT  0x05000000
#define JGT  0x06000000
#define RET  0x07000000
#define JMP  0x08000000
#define CALL 0x09000000

#define MOV  0x52000000
#define PUSH 0x42000000
#define POP  0x45000000
#define INC  0x48000000
#define DEC  0x4b000000
#define NOT  0x4e000000
#define LEA  0x5e000000
#define CMP  0x60000000

#define ADD  0x70000000
#define SUB  0x80000000
#define MUL  0x90000000
#define DIV  0xa0000000
#define SHL  0xb0000000
#define SHR  0xc0000000
#define AND  0xd0000000
#define OR   0xe0000000
#define XOR  0xf0000000


char argstr[20];
int  type,offlit,adr,prog_counter;
int  labelcount,maxlabel=0,valuecount,maxvalue=0;
char label[500][50];
char value[500][50];
int  labeladdr[500];
int  valueaddr[500];
FILE *fpout;


void analyze(void) {
	
	int i,l1,pos,valuepos;
	char str[20];
	
// 		argtype: 1 = lit, 2 = adr, 3 = (adr), 4 = off(adr)
	
// 36 = $	   					   
	   if (argstr[0] == 36) 
	   { 
	   		type = 1;
			sscanf( argstr+1, "%d", &offlit ); 
			adr = -1;
			return;
	   }	
	   
// 37 = %
	   if (argstr[0] == 37 )
	   { 
			type = 2;
			l1 = strlen(argstr);
		    if (l1==2) 
			{
				strncpy( str, argstr+1, 1 ); adr = atoi(str); 
			}
	   		if (l1==3) 
			{
				strncpy( str, argstr+1, 2 ); adr = atoi(str);  
				if (adr==0) {if (argstr[1]==66) adr=16; else adr=17;} //66 = B
			}
			adr = adr + regisgteroffset;
			offlit = -1;
			return;
		}	 
			  					   
// ( = 40	   
	   if (argstr[0] == 40) 
	   { 
	   		type = 3;
	   		l1 = strlen(argstr);
			if (l1==4) 
			{
				strncpy( str, argstr+2, 1 ); adr = atoi(str);  
			}
	   		if (l1==5) 
			{
				strncpy( str, argstr+2, 2 ); adr = atoi(str);
				if (adr==0) {if (argstr[2]==66) adr=16; else adr=17;}  //66 = B
			}
			adr = adr + regisgteroffset;
			offlit = 0;
			return;
	   }
	   
// - = 45	   					or 1-9 
	   if ((argstr[0] == 45) || (argstr[0] >= 49 && argstr[0]<=57))
	   {
	   		type = 4;
			sscanf( argstr, "%d", &offlit);
			pos = strcspn( argstr, "(" );
			l1 = strlen(argstr);
			l1 = l1-pos-3;
			if (l1==1) 
			{
				strncpy( str, argstr+pos+2, 1 ); adr = atoi(str);  
			}
	   		if (l1==2) 
			{
				strncpy( str, argstr+pos+2, 2 ); adr = atoi(str);
				if (adr==0) {if (argstr[pos+2]==66) adr=16; else adr=17;}  //66 = B
			}
			adr = adr + regisgteroffset;
			return;
	   }
	   
// global variable?
	   type = 2; 			    //if non of above a global variable is assumed
	   
	   	// find global variable adress, probably only defined in the second run
	   valuepos=-1;
	   for (i=0;i<maxvalue; i++) if (!strcmp (argstr, value[i])) valuepos = i;
	   if (valuepos>-1) adr = valueaddr[valuepos];
	   offlit = -1;
}

void out3arg(int cmd, int arg1, int arg2, int arg3, int offlit1, int offlit2, int offlit3, int adr1, int adr2, int adr3) {
	int command;
	
	   
	   if (arg1==1 && arg2==1 && arg3==2) {		//LIT1/LIT2/adr
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   if (arg1==2 && arg2==1 && arg3==2) {		//adr1/LIT/adr2
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==3 && arg2==1 && arg3==2) {		//(adr)/LIT/adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   
	   if (arg1==4 && arg2==1 && arg3==2) {		//O(adr)/LIT/adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==1 && arg2==2 && arg3==2) {		//LIT/adr1/adr2
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==2 && arg2==2 && arg3==2) {		//adr1/adr2/adr3
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   
	   if (arg1==3 && arg2==2 && arg3==2) {		//(adr)/adr1/adr2
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==4 && arg2==2 && arg3==2) {		//O(adr)/adr1/adr2
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==1 && arg2==3 && arg3==2) {		//LIT/(adr)/adr
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   
	   if (arg1==2 && arg2==3 && arg3==2) {		//adr1/(adr)/adr2
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==3 && arg2==3 && arg3==2) {		//(adr1)/(adr2)/adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==4 && arg2==3 && arg3==2) {		//O(adr)/(adr)/adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==1 && arg2==4 && arg3==2) {		//LIT/O(adr)/adr
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   
	   if (arg1==2 && arg2==4 && arg3==2) {		//adr1/O(adr)/adr2
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==3 && arg2==4 && arg3==2) {		//(adr)/O(adr)/adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
	   if (arg1==4 && arg2==4 && arg3==2) {		//O(adr1)/O(adr2)/adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr3); prog_counter++;
							   }
}

void out2arg(int cmd, int arg1, int arg2, int offlit1, int offlit2, int adr1, int adr2) {
	int command;
	
	   // MOV adr1, lit		or		MOV adr1, off1, lit		or		MOV adr1, off1, adr2, off2
	   
	   if (arg1==1 && arg2==2) {		//MOV lit -> adr
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
							   }
	   if (arg1==1 && arg2==3) {		//MOV lit -> (adr)
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
							   }
	   if (arg1==1 && arg2==4) {		//MOV lit -> off(adr)
	   							command = cmd + adr2; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
							   }
	   
	   if (arg1==2 && arg2==2) {		//MOV adr1 -> adr2
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==2 && arg2==3) {		//MOV adr1 -> (adr2)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==2 && arg2==4) {		//MOV adr1 -> off(adr2)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   
	   if (arg1==3 && arg2==2) {		//MOV (adr1) -> adr2
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==3 && arg2==3) {		//MOV (adr1) -> (adr2)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==3 && arg2==4) {		//MOV (adr1) -> off(adr2)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   
	   if (arg1==4 && arg2==2) {		//MOV off(adr1) -> adr2
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==4 && arg2==3) {		//MOV off(adr1) -> (adr2)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==4 && arg2==4) {		//MOV off(adr1) -> off(adr2)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
}

void outCMParg(int cmd, int arg1, int arg2, int offlit1, int offlit2, int adr1, int adr2) {
	int command;
	   
	   if (arg1==1 && arg2==1) {		//CMP lit1,lit2
	   							command = cmd; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   if (arg1==2 && arg2==1) {		//CMP adr,lit
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   if (arg1==3 && arg2==1) {		//CMP (adr),lit
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   
	   if (arg1==4 && arg2==1) {		//CMP off(adr),lit
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   if (arg1==1 && arg2==2) {		//CMP lit,adr
	   							command = cmd; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==2 && arg2==2) {		//CMP adr,adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   
	   if (arg1==3 && arg2==2) {		//CMP (adr),adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==4 && arg2==2) {		//CMP off(adr),adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==1 && arg2==3) {		//CMP LIT/(adr)
	   							command = cmd; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   
	   if (arg1==2 && arg2==3) {		//CMP adr/(adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==3 && arg2==3) {		//CMP (adr)/(adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==4 && arg2==3) {		//CMP O(adr)/(adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
							   }
	   if (arg1==1 && arg2==4) {		//CMP LIT/O(adr)
	   							command = cmd; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   if (arg1==2 && arg2==4) {		//CMP adr/O(adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   if (arg1==3 && arg2==4) {		//CMP (adr)/O(adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
	   if (arg1==4 && arg2==4) {		//CMP O(adr)/O(adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit2); prog_counter++;
							   }
}

void out1arg(int cmd, int arg1, int offlit1, int adr1) {
	int command;
	   
	   if (arg1==2 ) {		// adr
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
							   }
	   if (arg1==3 ) {		//  (adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
							   }
	   if (arg1==4 ) {		//  off(adr)
	   							command = cmd + adr1; fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++; 
								fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
							   }
}


int main () {
   char str[50],str1[50];
   int pos,pos1,i,labelpos,run,arg1,arg2,arg3,adr1,adr2,adr3,offlit1,offlit2,offlit3;
   int jmpaddress, command;
   size_t laenge;
   FILE * fpin;
   
// 		argtype: 1 = lit, 2 = adr, 3 = (adr), 4 = off(adr)

   for (i=0;i<500;i++) labeladdr[i] = -1;
   
   for (run=0;run<2;run++) {    // two runs to find all labels
   	
   prog_counter=0;
   labelcount=0;
   valuecount=0;

   fpin = fopen ("file3.txt", "r");
   fpout = fopen ("cpu.txt", "w");
   while (fscanf(fpin, "%s", str) == 1) {
   //if (run==1) printf("in %s\n", str);


// label: check for ":"
   laenge = strlen(str);
   if (str[laenge-1]==58)
   {
   	if (run==0) {
   	
   	laenge = strlen(str);
	strncpy( label[labelcount], str, laenge-1);
   	//printf("%s\n", label[labelcount]);
   	labeladdr[labelcount] = prog_counter;
   	labelcount++;
   	}
   }
   
   
// .word
   if (!strcmp (str, ".word")) 
   {
   	fscanf(fpin, "%d", &adr1);
	   	
	if (run==0) {
		
	labelcount--;
	laenge = strlen(label[labelcount]);
	strncpy( value[valuecount], label[labelcount], laenge);
	valueaddr[valuecount] = valuecount + globalvaloffset;
	valuecount++;
	 }
   }
   
// POP,PUSH
   if (!strcmp (str, "POP") || !strcmp (str, "PUSH")) 
   {
   	if (run==1) printf("%s: ",str);
   	fscanf(fpin, "%s", str1);

   	
	   pos = strcspn( str1, "," );
	   laenge = strlen(str1);
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str1, pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x\n", type,offlit,adr);
	   arg1 = type; offlit1=offlit; adr1=adr;
	   
	   if (!strcmp (str, "POP"))  {
									    command = (3-1)+(arg1-2)*4;
	   									command = command<<24 ;
	   									command = MOV + command;
	   									out2arg(command,3,arg1,0,offlit1,17 + regisgteroffset,arg1);  //MOV (%SP) -> arg1
	   									command = INC + 17 + regisgteroffset;
										fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;  //INC %SP (%SP = 17+regisgteroffset)
	   								}		
	   
	   
	   if (!strcmp (str, "PUSH")) 	{
	  								 	command = DEC + 17 + regisgteroffset;
										fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;  //DEC %SP (%SP = 17+regisgteroffset)
									    command = (arg1-1)+(3-2)*4;
	   									command = command<<24 ;
	   									command = MOV + command;
	   									out2arg(command,arg1,3,offlit1,0,adr1,17 + regisgteroffset);  //MOV arg1 -> (%SP)
	   								}				
   }
   
// INC,DEC,NOT
   if (!strcmp (str, "INC") || !strcmp (str, "DEC") || !strcmp (str, "NOT")) 
   {
   	if (run==1) printf("%s: ",str);
   	fscanf(fpin, "%s", str1);

   	
	   pos = strcspn( str1, "," );
	   laenge = strlen(str1);
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str1, pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x\n", type,offlit,adr);
	   arg1 = type; offlit1=offlit; adr1=adr;
	   
	   command = arg1-2;
	   command = command<<24 ;
	   
	   // via ADD/SUB INC %n = ADD $1,%n,%help; MOV
	   if (!strcmp (str, "INC"))  command = INC  + command;
	   if (!strcmp (str, "DEC"))  command = DEC  + command;
	   if (!strcmp (str, "NOT"))  command = NOT  + command;
	   out1arg(command,arg1,offlit1,adr1);
   }
   
   
// RET
   if (!strcmp (str, "RET")) 
   {
   	if (run==1) printf("RET\n");
	   
	   fprintf(fpout,"%8.8x %8.8x\n", prog_counter,RET+0x00100000+17); prog_counter++;
	   fprintf(fpout,"%8.8x %8.8x\n", prog_counter,1); prog_counter++;
   }
   
   
// CALL
   if (!strcmp (str, "CALL") ) 
   {
   	if (run==1) printf("%s: ",str);
   	fscanf(fpin, "%s", str1);

   	
	   // find jump adress, probably only defined in the second run
	   labelpos=-1; jmpaddress=-1;
	   for (i=0;i<maxlabel; i++) if (!strcmp (str1, label[i])) labelpos = i;
	   if (labelpos>-1) jmpaddress = labeladdr[labelpos];

	  	command = DEC + 17 + regisgteroffset;
		fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;  	//DEC %SP (%SP = 17+regisgteroffset)
		command = (1-1)+(3-2)*4;
	   	command = command<<24 ;
	   	command = MOV + command;
	   	out2arg(command,1,3,prog_counter+2,0,0,17 + regisgteroffset);  			//MOV prog_counter+2 -> (%SP)

	    command = JMP + jmpaddress;
	   
	   if (run==1) printf("command %x\n",command);
	   
	   fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
   }
   
// JMP,JGE,JEQ,JNE,JLE,JLT,JGT
   if (!strcmp (str, "JGE") || !strcmp (str, "JEQ") || !strcmp (str, "JNE") || !strcmp (str, "JLE") || !strcmp (str, "JLT") || !strcmp (str, "JGT") || !strcmp (str, "JMP")) 
   {
   	if (run==1) printf("%s: ",str);
   	fscanf(fpin, "%s", str1);

   	
	   // find jump adress, probably only defined in the second run
	   labelpos=-1; jmpaddress=-1;
	   for (i=0;i<maxlabel; i++) if (!strcmp (str1, label[i])) labelpos = i;
	   if (labelpos>-1) jmpaddress = labeladdr[labelpos];
	   
	   if (!strcmp (str, "JMP"))  command = JMP +  jmpaddress;
	   if (!strcmp (str, "JGE"))  command = JGE +  jmpaddress;
	   if (!strcmp (str, "JEQ"))  command = JEQ +  jmpaddress;
	   if (!strcmp (str, "JNE"))  command = JNE +  jmpaddress;
	   if (!strcmp (str, "JLE"))  command = JLE +  jmpaddress;
	   if (!strcmp (str, "JLT"))  command = JLT +  jmpaddress;
	   if (!strcmp (str, "JGT"))  command = JGT +  jmpaddress;
	   
	   if (run==1) printf("command %x\n",command);
	   
	   fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
   }
   
// MOV
   if (!strcmp (str, "MOV") ) 
   {
   	if (run==1) printf("MOV: ");
   	fscanf(fpin, "%s", str);

   	
	   pos = strcspn( str, "," );
	   laenge = strlen(str);
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str, pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x; ", type,offlit,adr);
	   arg1 = type; offlit1=offlit; adr1=adr;
	   
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str+pos+1, laenge-pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x\n", type,offlit,adr);
	   arg2 = type; offlit2=offlit; adr2=adr;
	   
	   // MOV adr1, lit		or		MOV adr1, off1, lit		or		MOV adr1, off1, adr2, off2
	   
	   command = (arg1-1)+(arg2-2)*4;
	   command = command<<24 ;
	   command = MOV + command;
	   out2arg(command,arg1,arg2,offlit1,offlit2,adr1,adr2);
	   
   }
   
// CMP
   if (!strcmp (str, "CMP") ) 
   {
   	if (run==1) printf("CMP: ");
   	fscanf(fpin, "%s", str);

   	
	   pos = strcspn( str, "," );
	   laenge = strlen(str);
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str, pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x; ", type,offlit,adr);
	   arg1 = type; offlit1=offlit; adr1=adr;
	   
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str+pos+1, laenge-pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x\n", type,offlit,adr);
	   arg2 = type; offlit2=offlit; adr2=adr;
	   
	   command = (arg1-1)+(arg2-1)*4;
	   command = command<<24 ;
	   command = CMP + command;
	   outCMParg(command,arg1,arg2,offlit1,offlit2,adr1,adr2);
	   

   }
   
// LEA
   if (!strcmp (str, "LEA") ) 
   {
   	if (run==1) printf("LEA: ");
   	fscanf(fpin, "%s", str);

   	
	   pos = strcspn( str, "," );
	   laenge = strlen(str);
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str, pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x; ", type,offlit,adr);
	   arg1 = type; offlit1=offlit; adr1=adr;
	   
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str+pos+1, laenge-pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x\n", type,offlit,adr);
	   arg2 = type; offlit2=offlit; adr2=adr;
	   
	   if (arg1==3 && arg2==2) {
	   							command = LEA + adr1;
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2); prog_counter++;
	   							}
	   if (arg1==4 && arg2==2) {
	   							command = LEA + 0x01000000 + adr1;
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,offlit1); prog_counter++;
	   							fprintf(fpout,"%8.8x %8.8x\n", prog_counter,adr2   ); prog_counter++;
	   							}
   }
   
// ADD,SUB,MUL,DIV,AND,OR,XOR,SHL,SHR
   if (!strcmp (str, "ADD") || !strcmp (str, "SUB") || !strcmp (str, "MUL") || !strcmp (str, "DIV") || !strcmp (str, "AND") || !strcmp (str, "OR") || !strcmp (str, "XOR") || !strcmp (str, "SHL") || !strcmp (str, "SHR")) 
   {
   	if (run==1) printf("%s: ",str);
   	fscanf(fpin, "%s", str1);

   	laenge = strlen(str1);
	pos = strcspn( str1, "," );
   	pos1 = strcspn( &str1[pos+1], "," ); 
   	pos1 = pos + pos1 + 1;
	   
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str1, pos);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x; ", type,offlit,adr);
	   arg1 = type; offlit1=offlit; adr1=adr;
	   
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str1+pos+1, pos1-pos-1);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x; ", type,offlit,adr);
	   arg2 = type; offlit2=offlit; adr2=adr;
	   
	   memset(argstr, '\0', sizeof(argstr));
	   strncpy( argstr, str1+pos1+1, laenge-pos1);
	   if (run==1) printf("%s: ", argstr);
	   type = -1;
	   analyze();
	   if (run==1) printf("argtype: %d, offlit: %8.8x, adr: %8.8x\n", type,offlit,adr);
	   arg3 = type; offlit3=offlit; adr3=adr;

	   command = (arg1-1)+(arg2-1)*4;
	   command = command<<24 ;
	   if (!strcmp (str, "ADD")) command = ADD + command;
	   if (!strcmp (str, "SUB")) command = SUB + command;
	   if (!strcmp (str, "AND")) command = AND + command;
	   if (!strcmp (str, "OR"))  command = OR  + command;
	   if (!strcmp (str, "XOR")) command = XOR + command;
	   if (!strcmp (str, "ADD") || !strcmp (str, "SUB") || !strcmp (str, "AND") || !strcmp (str, "OR") || !strcmp (str, "XOR"))
	   		out3arg(command,arg1,arg2,arg3,offlit1,offlit2,offlit3,adr1,adr2,adr3);
	   		
	   if (!strcmp (str, "MUL") || !strcmp (str, "DIV"))
	   {
	   		command = (arg1-1)+(2-2)*4;
	   		command = command<<24 ;
	   		command = MOV + command;
	   		out2arg(command,arg1,2,offlit1,0,adr1,inarg1);  // MOV arg1 -> inarg1
	   			   		
			command = (arg2-1)+(2-2)*4;
	   		command = command<<24 ;
	   		command = MOV + command;
	   		out2arg(command,arg2,2,offlit2,0,adr2,inarg2);  // MOV arg2 -> inarg2
	   		
	   		if (!strcmp (str, "MUL")) command = CALL + MULadr;
	   		if (!strcmp (str, "DIV")) command = CALL + DIVadr;
			fprintf(fpout,"%8.8x %8.8x\n", prog_counter,command); prog_counter++;  //Call MUL or DIV subroutine
			
			command = (2-1)+(arg3-2)*4;
	   		command = command<<24 ;
	   		command = MOV + command;
	   		out2arg(command,2,arg3,0,offlit3,outarg1,adr3);  // MOV outarg1 -> adr3
	   		
			command = (2-1)+(2-2)*4;
	   		command = command<<24 ;
	   		command = MOV + command;
	   		out2arg(command,2,2,0,0,outarg2,12+regisgteroffset);  // MOV outarg2 -> %12
	   	}
	   	
	   if (!strcmp (str, "SHL") || !strcmp (str, "SHR"))
	   {

	   	}
   }
   
   }
   
   if (run==0) {
   				maxlabel = labelcount;
				maxvalue = valuecount;
				printf("labels and addresses:\n");
				for (i=0;i<maxlabel; i++) printf("%d: %-15s at(hex)   %x\n",i,label[i],labeladdr[i]);
				printf("\n");
				printf("variables and addresses:\n");
				for (i=0;i<maxvalue; i++) printf("%d: %-15s at(hex)   %x\n",i,value[i],valueaddr[i]);
				printf("\n");
				}
   
   fclose(fpin); fclose(fpout);
   }
   
   return(0);
}
