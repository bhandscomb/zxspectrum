/* zxf_protos.h */

LONG zxfReadFile(ZXFILE *,char *,ULONG);
LONG zxfWriteFile(ZXFILE *,char *,ULONG);
LONG zxfFree(ZXFILE *,ULONG);

LONG zxfCreate(ZXFILE **,char *,ULONG);
LONG zxfInsert(ZXFILE *,UBYTE *,ULONG);
