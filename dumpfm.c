/* dumpfm.c */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>

__unaligned struct fmhdr {
	UBYTE	date[3];
	UBYTE	unk1[74];
	UBYTE	numaccounts;
	UBYTE	unk2[3];
	UBYTE	numdescrip;
	UBYTE	unk3[11];
	};

struct fmtxt {
	UBYTE	len;
	UBYTE	num;
	char	txt[16];	/* max=14 */
	};

__unaligned struct fmtrn {
	UBYTE	date[3];
	UBYTE	acc1;
	UBYTE	acc2;
	UBYTE	descr;
	UBYTE	pounds[3];
	UBYTE	pence;
	UBYTE	flags;
	};

#define FM_END_MARKER	0xab

FILE *fp=NULL;

struct fmhdr hdr;
struct fmtxt *acc=NULL,*des=NULL;
struct fmtrn trn;

void error(char *msg) {
  if(fp) fclose(fp);
  if(acc) free(acc);
  if(des) free(des);
  puts(msg);
  exit(1);
  }

void find(int number,struct fmtxt *db,int dbsize) {
  int loop;
  for(loop=0;loop<dbsize;loop++)
    if(db[loop].num==number) {
      printf("%s",db[loop].txt);
      return;
      }
  printf("              ");
  }

int main(int argc,char *argv[]) {
  int loop;
  char blank[15]="              ";
  if(argc!=2)
    error("Invalid args.\nUsage: dumpfm <filename>");
  if((fp=fopen(argv[1],"rb"))==NULL)
    error("Unable to open file");
  if(fgetc(fp)!=0xff)
    error("'Bytes' marker not found");
  if(fread(&hdr,sizeof(struct fmhdr),1,fp)!=1)
    error("Unable to read header");
  if(hdr.numaccounts) {
    if((acc=malloc(sizeof(struct fmtxt)*hdr.numaccounts))==NULL)
      error("Unable to allocate memory for accounts");
    for(loop=0;loop<hdr.numaccounts;loop++) {
      if(fread(&acc[loop],2,1,fp)!=1)
        error("[ERR_A1]");
      strcpy(acc[loop].txt,blank);
      if(fread(acc[loop].txt,(acc[loop].len&0x7f)-2,1,fp)!=1)
        error("[ERR_A2]");
      }
    }
  if(hdr.numdescrip) {
    if((des=malloc(sizeof(struct fmtxt)*hdr.numdescrip))==NULL)
      error("Unable to allocate memory for descriptions");
    for(loop=0;loop<hdr.numdescrip;loop++) {
      if(fread(&des[loop],2,1,fp)!=1)
        error("[ERR_D1]");
      strcpy(des[loop].txt,blank);
      if(fread(des[loop].txt,(des[loop].len&0x7f)-2,1,fp)!=1)
        error("[ERR_D2]");
      }
    }
  for(;;) {
    if(fread(&trn,1,1,fp)!=1)
      error("[ERR_T1]");
    if(trn.date[0]==FM_END_MARKER)
      break;
    if(fread(&trn.date[1],sizeof(struct fmtrn)-1,1,fp)!=1)
      error("[ERR_T2]");
    printf("%02x/%02x/%02x ",trn.date[2],trn.date[1],trn.date[0]);
    find(trn.acc1,acc,hdr.numaccounts);
    printf(" ");
    find(trn.acc2,acc,hdr.numaccounts);
    printf(" ");
    find(trn.descr,des,hdr.numdescrip);
    printf (
      " %02x%02x%02x.%02x%c 0x%02x\n",
      trn.pounds[0]&0x7f,trn.pounds[1],trn.pounds[2],
      trn.pence,
      trn.pounds[0]&0x80?'-':'+',
      trn.flags
      );
    }
  fclose(fp);
  }
