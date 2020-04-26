/* zxf_wrf.c - ZX File WRite File */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>

#include "zxf.h"
#include "zxf_tag.h"

void byteswap(UWORD *x) {
  *x=((*x&0xff00)>>8)|((*x&0x00ff)<<8);
  }

void hdrswap(ZXFILE *ZXF) {
  byteswap(&ZXF->zxf_Header->zxf_BytesSize);
  if(ZXF->zxf_Header->zxf_Type==0) {
    byteswap(&ZXF->zxf_Header->zxf_Line);
    byteswap(&ZXF->zxf_Header->zxf_ProgramSize);
    }
  if(ZXF->zxf_Header->zxf_Type==3)
    byteswap(&ZXF->zxf_Header->zxf_Address);
  }

LONG zxfinit(FILE **fp,char *fname,int type) {
  if((*fp=fopen(fname,"wb"))==NULL)
    return(ZXF_NOFILE);
  fputc(type,*fp);
  }

LONG zxfwrite(FILE *fp,UBYTE *block,int type,int filesize) {
  UBYTE *ptr,chk;
  int loop=0;
  chk=type;
  if(fwrite(block,filesize,1,fp)!=1) {
    fclose(fp); fp=NULL;
    return(ZXF_WRITEERROR);
    }
  ptr=block;
  for(;loop<filesize;loop++)
    chk^=*ptr++;
  fputc(chk,fp);
  fclose(fp); fp=NULL;
  return(ZXF_OKAY);
  }

LONG zxfWriteFile(ZXFILE *ZXF,char *name,ULONG flags) {
  char fname[256];
  FILE *fp=NULL;
  LONG rc;
  if(ZXF==NULL)
    return(ZXF_NULLZXFILE);
  if(ZXF->zxf_Tag!=ZXFTAG)
    return(ZXF_NOTAG);
  if((flags&ZXF_ALLBLOCKS)==0)
    return(ZXF_INSANE);
  if(ZXF->zxf_Header==NULL)
    return(ZXF_NOHEADER);
  if(((flags&ZXF_EVERYTHING)==ZXF_EVERYTHING)&&(ZXF->zxf_Bytes==NULL))
    flags&=~ZXF_BYTES;
  if((ZXF->zxf_Bytes==NULL)&&(flags&ZXF_BYTES))
    return(ZXF_NOBLOCK);
  if(flags&ZXF_HEADER) {
    if(flags&ZXF_ZXNAME) sprintf(fname,"%s.header",name);
    else strcpy(fname,name);
    rc=zxfinit(&fp,fname,0x00);
    if(rc!=ZXF_OKAY) return(rc);
    hdrswap(ZXF);
    rc=zxfwrite(fp,ZXF->zxf_Header,0x00,sizeof(struct zxfhdr));
    if(rc!=ZXF_OKAY) return(rc);
    hdrswap(ZXF);
    }
  if(flags&ZXF_BYTES) {
    if(flags&ZXF_ZXNAME) sprintf(fname,"%s.bytes",name);
    else strcpy(fname,name);
    rc=zxfinit(&fp,fname,0xff);
    if(rc!=ZXF_OKAY) return(rc);
    rc=zxfwrite(fp,ZXF->zxf_Bytes,0xff,ZXF->zxf_Header->zxf_BytesSize);
    if(rc!=ZXF_OKAY) return(rc);
    }
  return(ZXF_OKAY);
  }
