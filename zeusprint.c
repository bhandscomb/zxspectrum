/* zeusprint.c */

#include <stdio.h>
#include <stdlib.h>

char token[128][6]={
  "A","ADC ","ADD ","AF'","AF","AND ","B","BC",          /* 0x80-0x87 */
  "BIT ","C","CALL ","CCF","CP ","CPD","CPDR","CPI",     /* 0x88-0x8f */
  "CPIR","CPL","D","DAA","DE","DEC ","DEFB ","DEFM ",    /* 0x90-0x97 */
  "DEFS ","DEFW ","DI","DISP ","DJNZ ","E","EI","ENT ",  /* 0x98-0x9f */
  "EQU ","EX ","EXX","H","HALT","HL","I","IM",           /* 0xa0-0xa7 */
  "IN ","INC ","IND","INDR","INI","INIR","IX","IY",      /* 0xa8-0xaf */
  "JP ","JR ","L","LD ","LDD","LDDR","LDI","LDIR",       /* 0xb0-0xb7 */
  "M","NC","NEG","NOP","NV","NZ","OR ","ORG ",           /* 0xb8-0xbf */
  "OTDR","OTIR","OUT ","OUTD","OUTI","P","PE","PO",      /* 0xc0-0xc7 */
  "POP ","PUSH ","R","RES ","RET ","RETI","RETN","RL ",  /* 0xc8-0xcf */
  "RLA","RLC ","RLCA","RLD ","RR ","RRA","RRC ","RRCA",  /* 0xd0-0xd7 */
  "RRD ","RST ","SBC ","SCF","SET ","SLA ","SP","SRA ",  /* 0xd8-0xdf */
  "SRL ","SUB ","V","XOR ","Z","","","",                 /* 0xe0-0xe7 */
  "","","","","","","","",                               /* 0xe8-0xef */
  "","","","","","","","",                               /* 0xf0-0xf7 */
  "","","","","","","",""                                /* 0xf8-0xff */
  };

void error(char *);

FILE *ifp=NULL;

main(argc,argv)
  int argc;
  char *argv[];
  {
  int line,chr,spcloop,numspc;

  if(argc!=2)
    error("Usage: zeusprint <filename>");
  if((ifp=fopen(argv[1],"rb"))==NULL)
    error("fopen(rb)");
  fgetc(ifp);
  for(;;) {
    line=(fgetc(ifp))+fgetc(ifp)*256; /* forced evaluation order! */
    if(line==0xffff)
      break;
    printf("%05d ",line);
    for(;;) {
      chr=fgetc(ifp);
      if(chr<0)
        error("Unexpected EOF");
      if((chr>=0xe5)||((chr>=0x01)&&(chr<=0x1f)&&(chr!=0x0a)))
        printf("[0x%02x]",chr);
      if((chr>=0x80)&&(chr<=0xe4))
        printf("%s",token[chr-128]);
      if(chr==0x0a) {
        numspc=fgetc(ifp);
        for(spcloop=0;spcloop<numspc;spcloop++)
          putchar(' ');
        }
      if((chr>=0x20)&&(chr<=0x7f))
        if(chr==0x60) putchar('£');
        else if(chr==0x7f) putchar('©');
        else putchar(chr);
      if(chr==0) {
        putchar('\n');
        break;
        }
      }
    }
  fclose(ifp);
  }

void error(msg)
  char *msg;
  {
  if(ifp) fclose(ifp);
  printf("\nERROR: %s\n",msg);
  exit(1);
  }
