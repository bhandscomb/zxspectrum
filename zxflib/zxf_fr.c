/* zxf_fr.c - ZX File Free */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>

#include "zxf.h"
#include "zxf_tag.h"

LONG zxfFree(ZXFILE *ZXF,ULONG flags) {
  if(ZXF==NULL)
    return(ZXF_NULLZXFILE);
  if(ZXF->zxf_Tag!=ZXFTAG)
    return(ZXF_NOTAG);
  if(flags&ZXF_EVERYTHING) {
    if((flags&ZXF_EVERYTHING)==ZXF_EVERYTHING) {
      if(ZXF->zxf_Header==NULL) flags&=~ZXF_HEADER;
      if(ZXF->zxf_Bytes==NULL) flags&=~ZXF_BYTES;
      }
    if((flags&ZXF_HEADER)&&(ZXF->zxf_Header==NULL))
      return(ZXF_NOBLOCK);
    if((flags&ZXF_BYTES)&&(ZXF->zxf_Bytes==NULL))
      return(ZXF_NOBLOCK);
    if(flags&ZXF_HEADER) {
      free(ZXF->zxf_Header);
      ZXF->zxf_Header=NULL;
      }
    if(flags&ZXF_BYTES) {
      free(ZXF->zxf_Bytes);
      ZXF->zxf_Bytes=NULL;
      }
    if(flags&ZXF_ZXFILE) {
      if((ZXF->zxf_Header)||(ZXF->zxf_Bytes))
        return(ZXF_BLOCKEXISTS);
      ZXF->zxf_Tag=0;
      free(ZXF);
      ZXF=NULL;
      }
    }
  else return(ZXF_INSANE);
  return(ZXF_OKAY);
  }
