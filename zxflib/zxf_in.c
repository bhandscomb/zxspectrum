/* zxf_in.c - ZX File Insert */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>

#include "zxf.h"
#include "zxf_tag.h"

LONG zxfInsert(ZXFILE *ZXF,UBYTE *block,ULONG flags) {
  if(ZXF==NULL)
    return(ZXF_NULLZXFILE);
  if(ZXF->zxf_Tag!=ZXFTAG)
    return(ZXF_NOTAG);
  if(((flags&ZXF_ALLBLOCKS)==ZXF_ALLBLOCKS)||((flags&ZXF_ALLBLOCKS)==0))
    return(ZXF_INSANE);
  if(block==NULL)
    return(ZXF_NODATA);
  if (
    ((flags&ZXF_HEADER)&&(ZXF->zxf_Header)) ||
    ((flags&ZXF_BYTES)&&(ZXF->zxf_Bytes))
    )
    return(ZXF_BLOCKEXISTS);
  if(flags&ZXF_HEADER)
    ZXF->zxf_Header=block;
  if(flags&ZXF_BYTES)
    ZXF->zxf_Bytes=block;
  return(ZXF_OKAY);
  }
