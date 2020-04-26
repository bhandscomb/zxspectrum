/* baslist.c */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>

#include "zxflib/zxf.h"
#include "zxflib/zxf_protos.h"

char token[128][12]= {
	/* 0x80 to 0x8F - block graphics chars */
	"«G-»","«Gtr»","«Gtl»","«Gt»","«Gbr»","«Gr»","«Gtlbr»","«Gxbl»",
	"«Gbl»","«Gtrbl»","«Gl»","«Gxbr»","«Gb»","«Gxtl»","«Gxtr»","«G+»",
	/* 0x90 to 0xA4 - user defined graphics chars */
	"«GA»","«GB»","«GC»","«GD»","«GE»","«GF»","«GG»","«GH»",
	"«GI»","«GJ»","«GK»","«GL»","«GM»","«GN»","«GO»","«GP»",
	"«GR»","«GQ»","«GS»","«GT»","«GU»",
	/* 0xA5 to 0xFF - real tokens */
	"RND","INKEY$","PI",
	"FN","POINT","SCREEN$","ATTR","AT","TAB","VAL$","CODE",
	"VAL","LEN","SIN","COS","TAN","ASN","ACS","ATN",
	"LN","EXP","INT","SQR","SGN","ABS","PEEK","IN",
	"USR","STR$","CHR$","NOT","BIN"," OR"," AND","<=",
	">=","<>"," LINE"," THEN"," TO"," STEP","DEF FN","CAT",
	"FORMAT","MOVE","ERASE","OPEN #","CLOSE #","MERGE","VERIFY","BEEP",
	"CIRCLE","INK","PAPER","FLASH","BRIGHT","INVERSE","OVER","OUT",
	"LPRINT","LLIST","STOP","READ","DATA","RESTORE","NEW","BORDER",
	"CONTINUE","DIM","REM","FOR","GO TO","GO SUB","INPUT","LOAD",
	"LIST","LET","PAUSE","NEXT","POKE","PRINT","PLOT","RUN",
	"SAVE","RANDOMIZE","IF","CLS","DRAW","CLEAR","RETURN","COPY"
	};

ZXFILE *bas=NULL;

void error(char *msg,int rc) {
  LONG dummy;
  if(bas) dummy=zxfFree(bas,ZXF_EVERYTHING);
  puts(msg);
  exit(rc);
  }

void attr(char *type,UBYTE **ptr,int bytes) {
  int loop;
  printf("«%s ",type);
  for(loop=0;loop<bytes;loop++) {
    (*ptr)++;
    if(loop) printf(",%d",**ptr);
    else printf("%d",**ptr);
    }
  putchar('»');
  }

int main(int argc,char *argv[]) {
  LONG rc;
  UBYTE *ptr,*end,*lend;
  int llen,instring=0;
  if((argc<2)||(argc>3))
    error("Usage: baslist [-128] <ZXfilename>",1);
  if(!strcmp(argv[1],"-128")) {
    strcpy(token[0x23],"SPECTRUM");
    strcpy(token[0x24],"PLAY");
    argv++; argc--;
    }
  if((rc=zxfCreate(&bas,"",ZXF_ZXFILE))!=ZXF_OKAY)
    error("ZXF Init Error",1);
  if((rc=zxfReadFile(bas,argv[1],ZXF_HEADER|ZXF_ZXNAME))!=ZXF_OKAY)
    error("Error reading header",1);
  if(bas->zxf_Header->zxf_Type!=0)
    error("Invalid ZX file type",1);
  if((rc=zxfReadFile(bas,argv[1],ZXF_BYTES|ZXF_ZXNAME))!=ZXF_OKAY)
    error("Error reading bytes",1);
  ptr=bas->zxf_Bytes;
  end=ptr+bas->zxf_Header->zxf_ProgramSize;
  while(ptr<end) {
    /* line number */
    printf("%4d ",(256*(*ptr++))+(*ptr++));
    /* line length - forced evaluation order*/
    llen=((*ptr++))+256*(*ptr++);
    lend=ptr+llen-1;
    /* basic line */
    while(ptr<lend) {
      /* 'normal' characters */
      if((*ptr>=0x20)&&(*ptr<=0x7E))
        if(*ptr==0x60) putchar('£');
        else putchar(*ptr);
      if(*ptr==0x7F) putchar('©');
      if((*ptr>=0x80)&&(*ptr<=0xFF)) {
        printf("%s",token[*ptr-0x80]);
        if((*ptr<0xC7)||(*ptr>0xC9))
          putchar(' ');
        }
      if(*ptr=='\"')
        instring^=1;
      if((*ptr==':')&&(instring==0))
        printf("\n     ");
      /* check for 'unused' and 'control' characters */
      switch(*ptr) {
        case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
        case 0x05: case 0x0F: case 0x18: case 0x19: case 0x1A:
        case 0x1B: case 0x1C: case 0x1D: case 0x1E: case 0x1F:
          printf("«%02x»",*ptr);
        case 0x06: printf("«PRINT comma»"); break;
        case 0x07: printf("«EDIT»"); break;
        case 0x08: printf("«cursor left»"); break;
        case 0x09: printf("«cursor right»"); break;
        case 0x0A: printf("«cursor down»"); break;
        case 0x0B: printf("«cursor up»"); break;
        case 0x0C: printf("«DELETE»"); break;
        case 0x0D: printf("«ENTER»"); break;
        case 0x0E: ptr+=5; break;   /* number */
        case 0x10: attr("INK",&ptr,1); break;
        case 0x11: attr("PAPER",&ptr,1); break;
        case 0x12: attr("FLASH",&ptr,1); break;
        case 0x13: attr("BRIGHT",&ptr,1); break;
        case 0x14: attr("INVERSE",&ptr,1); break;
        case 0x15: attr("OVER",&ptr,1); break;
        case 0x16: attr("AT",&ptr,2); break;
        case 0x17: attr("TAB",&ptr,1); break;
        }
      ptr++;
      }
    /* ENTER - skipped */
    ptr++;
    putchar('\n');
    }
  error("Done.",0);
  }
