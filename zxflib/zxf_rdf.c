/* zxf_rdf.c - ZX File ReaD File */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>

#include "zxf.h"
#include "zxf_tag.h"

void byteswap(UWORD *x) {
  *x=((*x&0xff00)>>8)|((*x&0x00ff)<<8);
  }

LONG zxfinit(FILE **fp,char *fname,int type,int *filesize) {
  if((*fp=fopen(fname,"rb"))==NULL)
    return(ZXF_NOFILE);
  if(fgetc(*fp)!=type) {
    fclose(*fp); *fp=NULL;
    return(ZXF_STRANGEBLOCK);
    }
  fseek(*fp,0,SEEK_END);
  *filesize=ftell(*fp)-2;
  rewind(*fp);
  return(ZXF_OKAY);
  }

LONG zxfread(FILE *fp,UBYTE **block,int filesize) {
  UBYTE *ptr,chk,fchk;
  int loop=0;
  if((*block=calloc(1,filesize))==NULL) {
    fclose(fp); fp=NULL;
    return(ZXF_NOMEMORY);
    }
  chk=fgetc(fp);
  if(fread(*block,filesize,1,fp)!=1) {
    fclose(fp); fp=NULL;
    free(*block); *block=NULL;
    return(ZXF_READERROR);
    }
  fchk=fgetc(fp);
  ptr=*block;
  for(;loop<filesize;loop++)
    chk^=*ptr++;
  if(chk!=fchk) {
    fclose(fp); fp=NULL;
    free(*block); *block=NULL;
    return(ZXF_PARITYERROR);
    }
  fclose(fp); fp=NULL;
  return(ZXF_OKAY);
  }

LONG zxfReadFile(ZXFILE *ZXF,char *name,ULONG flags) {
  char fname[256];
  FILE *fp=NULL;
  int filesize;
  LONG rc;
  if(ZXF==NULL)
    return(ZXF_NULLZXFILE);
  if(ZXF->zxf_Tag!=ZXFTAG)
    return(ZXF_NOTAG);
  if((flags&ZXF_ALLBLOCKS)==0)
    return(ZXF_INSANE);
  if (
    ((flags&ZXF_HEADER)&&(ZXF->zxf_Header)) ||
    ((flags&ZXF_BYTES)&&(ZXF->zxf_Bytes))
    )
    return(ZXF_BLOCKEXISTS);
  if(((flags&ZXF_ALLBLOCKS)==ZXF_ALLBLOCKS)&&((flags&ZXF_ZXNAME)==0))
    return(ZXF_INSANE);
  if(flags&ZXF_HEADER) {
    if(flags&ZXF_ZXNAME) sprintf(fname,"%s.header",name);
    else strcpy(fname,name);
    rc=zxfinit(&fp,fname,0x00,&filesize);
    if(rc!=ZXF_OKAY) return(rc);
    if(filesize!=sizeof(struct zxfhdr)) {
      fclose(fp); fp=NULL;
      return(ZXF_SIZEMISMATCH);
      }
    rc=zxfread(fp,&ZXF->zxf_Header,filesize);
    if(rc!=ZXF_OKAY) return(rc);
    byteswap(&ZXF->zxf_Header->zxf_BytesSize);
    if(ZXF->zxf_Header->zxf_Type==0) {
      byteswap(&ZXF->zxf_Header->zxf_Line);
      byteswap(&ZXF->zxf_Header->zxf_ProgramSize);
      }
    if(ZXF->zxf_Header->zxf_Type==3)
      byteswap(&ZXF->zxf_Header->zxf_Address);
    }
  if(flags&ZXF_BYTES) {
    if(flags&ZXF_ZXNAME) sprintf(fname,"%s.bytes",name);
    else strcpy(fname,name);
    rc=zxfinit(&fp,fname,0xff,&filesize);
    if(rc!=ZXF_OKAY) return(rc);
    if((ZXF->zxf_Header)&&(filesize!=ZXF->zxf_Header->zxf_BytesSize)) {
      fclose(fp); fp=NULL;
      return(ZXF_SIZEMISMATCH);
      }
    rc=zxfread(fp,&ZXF->zxf_Bytes,filesize);
    if(rc!=ZXF_OKAY) return(rc);
    }
  return(ZXF_OKAY);
  }
