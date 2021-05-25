#include <stdio.h>
#include <stdlib.h>


int main () {
   char str[50], str1[10], str2[10], str3[10];
   int i,pos,pos1,pos2,offset1,offset2,reg1,reg2,l1,l2,l3,ll,lit1,lit2;
   int arg1,arg2,arg3;
   size_t laenge;
   FILE * fp;

   fp = fopen ("mov.txt", "r");
   while (fscanf(fp, "%s", str) == 1) {
   printf("in %s\n", str);


// MOV
   if (!strcmp (str, "MOV")) 
   {
   	printf("MOV\n");
   	fscanf(fp, "%s", str);
   	
   	if (1) {
   	
	   arg1 = arg2 = -1;
	   pos = strcspn( str, "," );
	   laenge = strlen(str);
	   l1 = pos;
	   l2 = laenge - pos - 1;
	   //printf("laenge: %d, komma pos: %d, l1: %d, l2: %d\n", laenge, pos, l1, l2);
	   
// .................................... arg1

// 36 = $	   					   
	   if (str[0] == 36) 
	   { 
	   		arg1 = 1;
			sscanf( str+1, "%d", &lit1 );
			//printf("lit1 %d\n", lit1); 
	   }	
	   
// 37 = %
	   if (str[0] == 37 )
	   { 
	   		arg1 = 2;
	   		reg1 = -1;
		    if (l1==2) 
			{
				strncpy( str1, str+1, 1 ); reg1 = atoi(str1); 
			}
	   		if (l1==3) 
			{
				strncpy( str1, str+1, 2 ); reg1 = atoi(str1);  
				if (reg1==0) {if (str[1]==66) reg1=16; else reg1=17;} //66 = B
			}
	    if (reg1<0 || reg1>17) arg1 = -1;
		//printf("reg1 %d\n", reg1);
		}	 
			  					   
// ( = 40	   
	   if (str[0] == 40) 
	   { 
	   		arg1 = 3;
	   		reg1 = -1;
			if (l1==4) 
			{
				strncpy( str1, str+2, 1 ); reg1 = atoi(str1);  
			}
	   		if (l1==5) 
			{
				strncpy( str1, str+2, 2 ); reg1 = atoi(str1);
				if (reg1==0) {if (str[2]==66) reg1=16; else reg1=17;}
			}
		if (reg1<0 || reg1>17) arg1 = -1;
	   //printf("inreg %d\n", reg1);
	   }
	   
// - = 45	   
	   if (str[0] == 45)
	   {
	   		arg1 = 4;
			sscanf( str, "%d", &offset1 );
			reg1 = -1;
			pos1 = strcspn( str, "(" );
			ll = pos-pos1-3;
			if (ll==1) 
			{
				strncpy( str1, str+pos1+2, 1 ); reg1 = atoi(str1);  
			}
	   		if (ll==2) 
			{
				strncpy( str1, str+pos1+2, 2 ); reg1 = atoi(str1);
				if (reg1==0) {if (str[pos1+2]==66) reg1=16; else reg1=17;}
			}
		if (reg1<0 || reg1>17) arg1 = -1;
	   //printf("inreg %d\n", reg1);
			//printf("offset1 %d\n", offset1);
	   }
	   
// .................................... arg2


// 37 = %
	   if (str[pos+1] == 37)
	   { 
	   		arg2 = 2;
	   		reg2 = -1;
		    if (l2==2) 
			{
				strncpy( str2, str+pos+2, 1 ); reg2 = atoi(str2); 
			}
	   		if (l2==3) 
			{
				strncpy( str2, str+pos+2, 2 ); reg2 = atoi(str2);  
				if (reg2==0) {if (str[pos+2]==66) reg2=16; else reg2=17;} //66 = B
			}
	    if (reg2<0 || reg2>17) arg2 = -1;
		//printf("reg2 %d\n", reg2);
		}
		
// ( = 40	   
	   if (str[pos+1] == 40) 
	   { 
	   		arg2 = 3;
	   		reg2 = -1;
			if (l2==4) 
			{
				strncpy( str1, str+pos+3, 1 ); reg2 = atoi(str1);  
			}
	   		if (l2==5) 
			{
				strncpy( str1, str+pos+3, 2 ); reg2 = atoi(str1);
				if (reg2==0) {if (str[pos+3]==66) reg2=16; else reg2=17;}
			}
		if (reg2<0 || reg2>17) arg2 = -1;
	   //printf("reg2 %d\n", reg2);
	   }
	   
// - = 45	   
	   if (str[pos+1] == 45)
	   {
	   		arg2 = 4;
			sscanf( str+pos+1, "%d", &offset2 );
			reg2 = -1;
			pos2 = strcspn( str+pos, "(" );
			pos1 = strcspn( str+pos, ")" );
			ll = pos1-pos2-2;
			if (ll==1) 
			{
				strncpy( str1, str+pos2+2, 1 ); reg2 = atoi(str1);  
			}
	   		if (ll==2) 
			{
				strncpy( str1, str+pos2+2, 2 ); reg2 = atoi(str1);
				if (reg2==0) {if (str[pos2+2]==66) reg2=16; else reg2=17;}
			}
		if (reg2<0 || reg2>17) arg2 = -1;
		
		
			//printf("offset2 %d\n", offset2);
	   }
	if (arg1==-1 || arg2==-1)   printf("MOV failure!\n");
	else
	{	
		printf("MOV  ");
		if (arg1==1) printf("literal  %d to ",lit1);
		if (arg1==2) printf("register %d to ",reg1);
		if (arg1==3) printf("register indirect %d to ",reg1);
		if (arg1==4) printf("register indirect %d with offset %d to ",reg1,offset1);
		
		if (arg2==2) printf("register %d\n",reg2);
		if (arg2==3) printf("register indirect %d\n",reg2);
		if (arg2==4) printf("register indirect %d with offset %d\n",reg2,offset2);
	}   
   }
   }
   
// @
   else if (str[0] == 64)  
   {
   	printf("at\n");
   }
// PUSH
   else if (!strcmp (str, "PUSH"))  
   {
   	printf("PUSH\n");
   	fscanf(fp, "%s", str);
   }
//SUB
   else if (!strcmp (str, "SUB"))  
   {
   	printf("SUB\n");
   	fscanf(fp, "%s", str);
   }
//CMP
   else if (!strcmp (str, "CMP"))  
   {
   	printf("CMP\n");
   	fscanf(fp, "%s", str);
   	
   	if (1) {
   	
	   arg1 = arg2 = -1;
	   pos = strcspn( str, "," );
	   laenge = strlen(str);
	   l1 = pos;
	   l2 = laenge - pos - 1;
	   //printf("laenge: %d, komma pos: %d, l1: %d, l2: %d\n", laenge, pos, l1, l2);
	   
// .................................... arg1

// 36 = $	   					   
	   if (str[0] == 36) 
	   { 
	   		arg1 = 1;
			sscanf( str+1, "%d", &lit1 );
			//printf("lit1 %d\n", lit1); 
	   }	
	   
// 37 = %
	   if (str[0] == 37 )
	   { 
	   		arg1 = 2;
	   		reg1 = -1;
		    if (l1==2) 
			{
				strncpy( str1, str+1, 1 ); reg1 = atoi(str1); 
			}
	   		if (l1==3) 
			{
				strncpy( str1, str+1, 2 ); reg1 = atoi(str1);  
				if (reg1==0) {if (str[1]==66) reg1=16; else reg1=17;} //66 = B
			}
	    if (reg1<0 || reg1>17) arg1 = -1;
		//printf("reg1 %d\n", reg1);
		}	 
			  					   
// ( = 40	   
	   if (str[0] == 40) 
	   { 
	   		arg1 = 3;
	   		reg1 = -1;
			if (l1==4) 
			{
				strncpy( str1, str+2, 1 ); reg1 = atoi(str1);  
			}
	   		if (l1==5) 
			{
				strncpy( str1, str+2, 2 ); reg1 = atoi(str1);
				if (reg1==0) {if (str[2]==66) reg1=16; else reg1=17;}
			}
		if (reg1<0 || reg1>17) arg1 = -1;
	   //printf("inreg %d\n", reg1);
	   }
	   
// - = 45	   
	   if (str[0] == 45)
	   {
	   		arg1 = 4;
			sscanf( str, "%d", &offset1 );
			reg1 = -1;
			pos1 = strcspn( str, "(" );
			ll = pos-pos1-3;
			if (ll==1) 
			{
				strncpy( str1, str+pos1+2, 1 ); reg1 = atoi(str1);  
			}
	   		if (ll==2) 
			{
				strncpy( str1, str+pos1+2, 2 ); reg1 = atoi(str1);
				if (reg1==0) {if (str[pos1+2]==66) reg1=16; else reg1=17;}
			}
		if (reg1<0 || reg1>17) arg1 = -1;
	   //printf("inreg %d\n", reg1);
			//printf("offset1 %d\n", offset1);
	   }
	   
// .................................... arg2


// 36 = $	   					   
	   if (str[pos+1] == 36) 
	   { 
	   		arg2 = 1;
			sscanf( str+pos+2, "%d", &lit2 );
			//printf("lit1 %d\n", lit2); 
	   }	
	   
// 37 = %
	   if (str[pos+1] == 37)
	   { 
	   		arg2 = 2;
	   		reg2 = -1;
		    if (l2==2) 
			{
				strncpy( str2, str+pos+2, 1 ); reg2 = atoi(str2); 
			}
	   		if (l2==3) 
			{
				strncpy( str2, str+pos+2, 2 ); reg2 = atoi(str2);  
				if (reg2==0) {if (str[pos+2]==66) reg2=16; else reg2=17;} //66 = B
			}
	    if (reg2<0 || reg2>17) arg2 = -1;
		//printf("reg2 %d\n", reg2);
		}
		
// ( = 40	   
	   if (str[pos+1] == 40) 
	   { 
	   		arg2 = 3;
	   		reg2 = -1;
			if (l2==4) 
			{
				strncpy( str1, str+pos+3, 1 ); reg2 = atoi(str1);  
			}
	   		if (l2==5) 
			{
				strncpy( str1, str+pos+3, 2 ); reg2 = atoi(str1);
				if (reg2==0) {if (str[pos+3]==66) reg2=16; else reg2=17;}
			}
		if (reg2<0 || reg2>17) arg2 = -1;
	   //printf("reg2 %d\n", reg2);
	   }
	   
// - = 45	   
	   if (str[pos+1] == 45)
	   {
	   		arg2 = 4;
			sscanf( str+pos+1, "%d", &offset2 );
			reg2 = -1;
			pos2 = strcspn( str+pos, "(" );
			pos1 = strcspn( str+pos, ")" );
			ll = pos1-pos2-2;
			if (ll==1) 
			{
				strncpy( str1, str+pos2+2, 1 ); reg2 = atoi(str1);  
			}
	   		if (ll==2) 
			{
				strncpy( str1, str+pos2+2, 2 ); reg2 = atoi(str1);
				if (reg2==0) {if (str[pos2+2]==66) reg2=16; else reg2=17;}
			}
		if (reg2<0 || reg2>17) arg2 = -1;
		
		
			//printf("offset2 %d\n", offset2);
	   }
	if (arg1==-1 || arg2==-1)   printf("MOV failure!\n");
	else
	{	
		printf("CMP  ");
		if (arg1==1) printf("literal  %d to ",lit1);
		if (arg1==2) printf("register %d to ",reg1);
		if (arg1==3) printf("register indirect %d to ",reg1);
		if (arg1==4) printf("register indirect %d with offset %d to ",reg1,offset1);
		
		if (arg2==1) printf("literal %d\n",lit2);
		if (arg2==2) printf("register %d\n",reg2);
		if (arg2==3) printf("register indirect %d\n",reg2);
		if (arg2==4) printf("register indirect %d with offset %d\n",reg2,offset2);
	}   
   }
   }
//JGE
   else if (!strcmp (str, "JGE"))  
   {
   	printf("JGE\n");
   	fscanf(fp, "%s", str);
   }
//ADD
   else if (!strcmp (str, "ADD"))  
   {
   	printf("ADD\n");
   	
   	fscanf(fp, "%s", str);
   	pos1 = strcspn( str, "," );
   	pos2 = strcspn( &str[pos1+1], "," );
   	pos2 = pos1 + pos2 + 1;
	laenge = strlen(str);
	   //printf("Read String1 |%s|\n", str1 );
	   printf("laengen und pos: %d  %d  %d  %d  %d\n", laenge, pos1, pos2, str[pos1], str[pos2]);
   }
//JMP
   else if (!strcmp (str, "JMP"))  
   {
   	printf("JMP\n");
   	fscanf(fp, "%s", str);
   }
// unknown command
   else
   {
   	printf("CAUTION: unknown command!\n");
   }
   
}
   fclose(fp);
   
   return(0);
}
