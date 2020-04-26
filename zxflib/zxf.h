/* zxf.h */


/*---------- structures ----------*/

/* internal - used in header structure */

struct zxfhdrP {
	UWORD	zxfP_Line;
	UWORD	zxfP_ProgramSize;
	};

struct zxfhdrN {
	UBYTE	zxfN_Pad1;
	UBYTE	zxfN_NArrayName;
	UWORD	zxfN_Pad2;
	};

struct zxfhdrC {
	UBYTE	zxfC_Pad1;
	UBYTE	zxfC_CArrayName;
	UWORD	zxfC_Pad2;
	};

struct zxfhdrB {
	UWORD	zxfB_Address;
	UWORD	zxfB_Pad;
	};

/* header structure */

__unaligned struct zxfhdr {
	UBYTE	zxf_Type;
	char	zxf_Name[10];
	UWORD	zxf_BytesSize;
	union	{
		struct zxfhdrP	zxf_Program;
		struct zxfhdrN	zxf_NumberArray;
		struct zxfhdrC	zxf_CharacterArray;
		struct zxfhdrB	zxf_Bytes;
		} zxf_Data;
	};

/* defines for quick access within union in header structure */

#define zxf_Line	zxf_Data.zxf_Program.zxfP_Line
#define zxf_ProgramSize	zxf_Data.zxf_Program.zxfP_ProgramSize
#define zxf_NArrayName	zxf_Data.zxf_NumberArray.zxfN_NArrayName
#define zxf_CArrayName	zxf_Data.zxf_CharacterArray.zxfC_CArrayName
#define zxf_Address	zxf_Data.zxf_Bytes.zxfB_Address

/* main ZX File access structure */

typedef struct ZXFILE {
	ULONG		zxf_Tag;		/* PRIVATE! */
	struct zxfhdr	*zxf_Header;
	UBYTE		*zxf_Bytes;
	} ZXFILE;


/*---------- flags - always last argument ----------*/

/* block select */

#define ZXF_HEADER	  1
#define ZXF_BYTES	  2	/* not valid for zxfCreate */
#define ZXF_ALLBLOCKS	  3	/* not valid for zxfInsert/zxfCreate */
#define ZXF_ZXFILE	  4	/* MUST state this in zxfCreate */
#define ZXF_EVERYTHING	  7	/* zxfFree specific - no 'missing' errors */

#define ZXF_TPROGRAM	  8	/* zxfCreate specific - file type */
#define ZXF_TNARRAY	 16	/* ditto */
#define ZXF_TCARRAY	 32	/* ditto */
#define ZXF_TBYTES	 64	/* ditto */
#define ZXF_TYPEMASK	120	/* all the above */

#define ZXF_ZXNAME	128	/* ZX name passed (no .header/.bytes) */


/*---------- errors ----------*/

#define ZXF_OKAY		0	/* successful return code */
#define ZXF_NOMEMORY		-1	/* not enough memory for operation */
#define ZXF_NOBLOCK		-2	/* block not present for operation */
#define ZXF_BLOCKEXISTS		-3	/* existing block(s) */
#define ZXF_NAMETOOLONG		-4	/* zxfCreate specific */
#define ZXF_NODATA		-5	/* zxfInsert, NULL data ptr */
#define ZXF_NOFILE		-6	/* zxf...File - file open failed */
#define ZXF_STRANGEBLOCK	-7	/* zxf...File - invalid blk type */
#define ZXF_PARITYERROR		-8	/* zxfReadFile - file corrupt */
#define ZXF_SIZEMISMATCH	-9	/* size of file looks wrong... */
#define ZXF_READERROR		-10	/* zxfReadFile - error reading */
#define ZXF_WRITEERROR		-11	/* zxfWriteFile - error writing */
#define ZXF_NOHEADER		-12	/* zxfWriteFile MUST be given hdr */

#define ZXF_INSANE		-20	/* strange request made */
#define ZXF_NULLZXFILE		-21	/* NULL ptr passed as ZXFILE */
#define ZXF_INITZXFILE		-22	/* valid ptr passed to zxfCreate */
#define ZXF_NOTAG		-23	/* no tag found - corrupt ZXFILE? */
