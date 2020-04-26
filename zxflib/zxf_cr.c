/* zxf_cr.c - ZX File Create */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>

#include "zxf.h"
#include "zxf_tag.h"

LONG zxfCreate(ZXFILE **ZXF,char *name,ULONG flags) {
  int filetype=0xff;
  char nullname[]="";
  if(*ZXF!=NULL)
    if((*ZXF)->zxf_Tag==ZXFTAG)
      return(ZXF_INITZXFILE);
  *ZXF=NULL;
  if(flags&ZXF_BYTES)
    return(ZXF_INSANE);
  if(flags&ZXF_TYPEMASK) {
    switch(flags&ZXF_TYPEMASK) {
      case ZXF_TPROGRAM: filetype=0; break;
      case ZXF_TNARRAY: filetype=1; break;
      case ZXF_TCARRAY: filetype=2; break;
      case ZXF_TBYTES: filetype=3; break;
      default: return(ZXF_INSANE);
      }
    }
  if(name==NULL)
    name=nullname;
  if(strlen(name)>10)
    return(ZXF_NAMETOOLONG);
  if(flags&ZXF_ZXFILE) {
    if(*ZXF=calloc(1,sizeof(ZXFILE)))
      (*ZXF)->zxf_Tag=ZXFTAG;
    else return(ZXF_NOMEMORY);
    }
  else return(ZXF_INSANE);
  if(flags&ZXF_HEADER) {
    if((*ZXF)->zxf_Header=calloc(1,sizeof(struct zxfhdr))) {
      int loop;
      for(loop=0;loop<9;loop++)
        if(name[loop]) (*ZXF)->zxf_Header->zxf_Name[loop]=name[loop];
        else for(;loop<9;loop++)
          (*ZXF)->zxf_Header->zxf_Name[loop]=' ';
      (*ZXF)->zxf_Header->zxf_Type=filetype;
      }
    else {
      (*ZXF)->zxf_Tag=0;
      free(*ZXF);
      *ZXF=NULL;
      return(ZXF_NOMEMORY);
      }
    }
  return(ZXF_OKAY);
  }
