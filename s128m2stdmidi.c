/* s128m2stdmidi.c */
/* Spectrum 128 Midi to Standard MIDI File */

/*
 * v1.0		03/08/94	Initial version
 * v1.1		28/12/94	BUG FIX - !comment! terminate didn't work...
 * v1.2		04/02/95	Modified Z command for >255 values
 * v1.3		29/04/95	Comment special "!@ ... !" added
 */

char VersionString[]="$VER: s128m2stdmidi 1.3 (29.04.95)";


/*
 * Input $() files containing Spectrum 128 PLAY strings (PLAY a$,b$,...)
 * Output Standard MIDI File (Format 1)
 *
 * Each input file produces one output track
 */

/*
 * Spectrum 128 PLAY String
 *
 * [0-9]	Duration digit
 * [a-gA-G]	Note specifier (l/case = low octave, u/case = high octave)
 * #		Sharpen (may be stacked to provide higher notes - eg ##B)
 * $		Flatten (may be stacked to provide lower notes - eg $$c)
 * _		Duration tie
 * &		Rest
 * (		Repeat start
 * )		Repeat end
 * !		Comment start/end
 * H		Halt command, stops PLAY
 * M		Channel usage command (1-63) ***AY***
 * N		Number separator, duration follows
 * O		Octave command (0-8)
 * T		Tempo command, must be in 1st play string (60-240)
 * U		Volume effect enable command ***AY***
 * V		Volume command (0-15)
 * W		Volume effect select command (0-7) ***AY***
 * X		Volume effect duration command (0-65535) ***AY***
 * Y		MIDI channel select command (1-16)
 * Z		MIDI byte (0-255)
 *
 *
 * Comment Special (@ in comment)
 *
 * C <cpr>	Copyright notice	!@C(C) 1995 Plinkey!
 * N <name>	track Name		!@NBassTrack!
 *						<cpr>/<name> = "text"
 * K <n><s><Mm>	Key signature		!@K1bM!
 *						<n> = number of signs in sig
 *						<s> = sign (b/#/-)
 *						<Mm> = major/minor
 * T <tsig>	Time signature		!@T12/8!
 *						<tsig> = "top/bottom"
 *
 *
 * Durations
 *
 * 1		Semi-quaver
 * 2		Dotted semi-quaver
 * 3		Quaver
 * 4		Dotted quaver
 * 5		Crotchet
 * 6		Dotted crotchet
 * 7		Minim
 * 8		Dotted minim
 * 9		Semi-breve
 * 10		Triplet semi-quaver
 * 11		Triplet quaver
 * 12		Triplet crotchet
 *
 *
 *     C D E F G A B C D E F G A B C D E F G A B C D E F G A B
 *     :             :             :             :
 *     :             :             :             :           O
 *     :             :             :             :        -O---
 *     :             :             :             :       O
 *  +--:-------------:-------------:-------------:-----O--------+
 *  |  :             :             :             :   O          |
 *  +--:-------------:-------------:-------------:-O------------+
 *  |  :             :             :             O              |
 *  +--:-------------:-------------:-----------O-:--------------+
 *  |  :             :             :         O   :              |
 *  +--:-------------:-------------:-------O-----:--------------+
 *  |  :             :             :     O       :              |
 *  +--:-------------:-------------:---O---------:--------------+
 *  |  :             :             : O           :              |
 *  |  :             :             O             :              |
 *  |  :             :           O :             :              |
 *  +--:-------------:---------O---:-------------:--------------+
 *  |  :             :       O     c d e f g a b C D E F G A B  | O5
 *  +--:-------------:-----O-------:----------------------------+
 *  |  :             :   O         :                            |
 *  +--:-------------:-O-----------:----------------------------+
 *  |  :             O             :                            |
 *  +--:-----------O-:-------------:----------------------------+
 *  |  :         O   c d e f g a b C D E F G A B    O4          |
 *  +--:-------O-----:------------------------------------------+
 *     :     O       :
 *    -:---O-        :
 *     : O           :
 *    -O-            :
 *     :             :
 *     c d e f g a b C D E F G A B    O3
 *
 *
 *
 * Example PLAY string:
 *
 * PLAY "O4N5c$bC9_5$E#B#B###B####B"
 *				O4	Octave 4 select
 *				N	Number follows (duration)
 *				5	Duration=Crotchet
 *				c	C below middle C
 *				$b	B flat below middle C
 *				C	middle C
 *				9_5	Duration=Semi-breve+Crotchet
 *				$E	E flat above middle C
 *					{Comment: Duration now Crotchet}
 *				#B	C above middle C
 *				#B	{Again}
 *				###B	D above C above middle C
 *				####B	E flat/D sharp above C above middle C
 */



/* standard stuff */

#include <stdio.h>
#include <stdlib.h>

#define	LONG	int
#define	ULONG	unsigned
#define	WORD	short
#define	UWORD	unsigned short
#define	BYTE	char
#define	UBYTE	unsigned char


/* MIDI file chunk IDs */

#define	idMThd	0x4d546864
#define	idMTrk	0x4d54726b


/* conversion system constants */

#define	DIVISION	96	/* 96 pulses per quarter note (crotchet) */
#define	MIDIBLOCK_LEN	1024	/* 1024 per midi file block */

#define	ERROR_MAGIC	0xABADC0DE	/* "A Bad Code" -  no # in err msg */


/* MIDI message creators */

#define	NOTE_OFF	(0x80+midichan-1)
#define	NOTE_ON		(0x90+midichan-1)


/* conversion system structures */

struct midiblock {
	struct midiblock *next;
	char data[MIDIBLOCK_LEN];
	};


/* files */

FILE *ifp=NULL,*ofp=NULL;


/* timing array - convert Spectrum duration values to MIDI pulses */

int spec2pulse[13]={	0,	/* unused */
			24,	/* semiQ */
			36,	/* semiQdot */
			48,	/* Q */
			72,	/* Qdot */
			96,	/* C */
			144,	/* Cdot */
			192,	/* M */
			288,	/* Mdot */
			384,	/* semiB */
			16,	/* semiQ3 */
			32,	/* Q3 */
			64	/* C3 */
			};

/* conv system vars */

int quiet=0;
struct midiblock *firstblock=NULL,*currblock=NULL;
int midiblockptr=MIDIBLOCK_LEN,numberofblocks=0;


/* parser vars */

int *valptr=NULL;	/* points to "current" value */
int delta,		/* delta time accumulated by rests (&) */
    duration,		/* Spectrum duration value */
    ignoresw,		/* 0 if not in comment, !0 if in comment (!) */
    midibyte,		/* MIDI programming byte (Z) */
    midichan,		/* MIDI output channed (Y) */
    octave,		/* octave range (O) */
    pitchshift,		/* sharp (+ve) / flag (-ve) counter (#/$) */
    tempo,		/* tempo (T) */
    tie,		/* MIDI Pulse duration tie value (_) */
    volume,		/* volume (V) */
    zzz;		/* non-MIDI buffer/scrap variable */


/* prototypes */

void convtrk(int);			/* datasize */
void error(char *,char *,int);		/* where,msg,number */
void frd(UBYTE *,ULONG,char *);		/* buffer,length,caller */
void freedata(void);
void fwr(UBYTE *,ULONG,char *);		/* buffer,length,caller */
void processval(void);
int dospecial(void);
void putmidi(ULONG,int);		/* data,size */
int strindex(char *,char);		/* s,c */
void usage(void);
void writeheader(UWORD);		/* tracks */
void writetrack(void);
void WriteVarLen(ULONG);		/* value */


/***************************************************************************/


int main(int argc,char *argv[]) {
  char fn[80];
  int trkcnt=0,trkloop,byteslength,xloop;
  if((argc<2)||(argc>3)) usage();
  if((argc==3)&&(strcmp(argv[2],"-q"))) usage();
  if(!strcmp(argv[2],"-q")) quiet=1;
  sprintf(fn,"%s.midi",argv[1]);
  if((ofp=fopen(fn,"wb"))==NULL)
    error("main","Couldn't open output file",ERROR_MAGIC);
  if(!quiet) puts("Counting tracks...");
  for(;;trkcnt++) {
    sprintf(fn,"%s%c.header",argv[1],'A'+trkcnt);
    if((ifp=fopen(fn,"rb"))!=NULL) {
      if(!quiet) printf("Found track %d (header)\n",trkcnt+1);
      fclose(ifp); ifp=NULL;
      }
    else break;
    }
  writeheader(trkcnt);
  for(trkloop=0;trkloop<trkcnt;trkloop++) {
    if(!quiet) printf("Processing track %d\n",trkloop+1);
    sprintf(fn,"%s%c.header",argv[1],'A'+trkloop);
    if((ifp=fopen(fn,"rb"))==NULL)
      error("main","Couldn't open header file for track %d",trkloop+1);
    for(xloop=0;xloop<12;xloop++) fgetc(ifp);
    byteslength=(fgetc(ifp))+fgetc(ifp)*256;
    if((feof(ifp))||(byteslength<0)||(byteslength>65535))
      error("main","Invalid header file for track %d",trkloop+1);
    fclose(ifp); ifp=NULL;
    sprintf(fn,"%s%c.bytes",argv[1],'A'+trkloop);
    if((ifp=fopen(fn,"rb"))==NULL)
      error("main","Couldn't open bytes file for track %d",trkloop+1);
    fgetc(ifp);
    convtrk(byteslength);
    fclose(ifp); ifp=NULL;
    writetrack();
    freedata();
    }
  fclose(ofp); ofp=NULL;
  return(0);
  }


/***************************************************************************/


void convtrk(int datasize) {
  int chrloop,inchr,note;
  char notestring[]="c.d.ef.g.a.bC.D.EF.G.A.B";
  char me[]="convtrk";
  delta=ignoresw=midibyte=pitchshift=tie=0;
  duration=octave=5;
  midichan=1;
  tempo=120;
  volume=15;
  for(chrloop=0;chrloop<datasize;chrloop++) {
    inchr=getc(ifp);
    if(inchr<0)
      error(me,"Unexpected EOF",ERROR_MAGIC);
    if(inchr>127)
      error(me,"Invalid char found - 0x%02x",inchr);
    if(!ignoresw)
      switch(inchr) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
          if(valptr==NULL) {
            valptr=&duration;
            duration=tie=0;
            }
          *valptr=(*valptr)*10+(inchr-'0');
          break;
        case 'c': case 'd': case 'e': case 'f': case 'g': case 'a': case 'b':
        case 'C': case 'D': case 'E': case 'F': case 'G': case 'A': case 'B':
          processval(); valptr=NULL;
          note=octave*12+strindex(notestring,inchr)+pitchshift;
          if((note<0)||(note>127))
            error(me,"Invalid note number generated!",ERROR_MAGIC);
          WriteVarLen(delta);
          putmidi(NOTE_ON,1);
          putmidi(note,1);
          putmidi((volume<<3)+(volume>>1),1);
          WriteVarLen(tie+spec2pulse[duration]);
          putmidi(NOTE_OFF,1);
          putmidi(note,1);
          putmidi(127,1);
          tie=pitchshift=delta=0;
          break;
        case '#':
          processval(); valptr=NULL;
          pitchshift++;
          break;
        case '$':
          processval(); valptr=NULL;
          pitchshift--;
          break;
        case '_':
          if(valptr!=&duration)
            error("convtrk","Tie found outside of duration def",ERROR_MAGIC);
          if((duration<1)||(duration>12))
            error("convtrk","Invalid duration found - %d",duration);
          tie+=spec2pulse[duration];
          duration=0;
          break;
        case '&':
          processval(); valptr=NULL;
          delta+=tie+spec2pulse[duration];
          tie=0;
          break;
        case '(':
        case ')':
          error(me,"Unsupported feature: '...(repeated)...'",ERROR_MAGIC);
          break;
        case '!':
          processval(); valptr=NULL;
          ignoresw=1;
          break;
        case 'H':
          processval(); valptr=NULL;
          break;
        case 'M':
          processval(); valptr=&zzz;
          break;
        case 'N':
          processval(); valptr=NULL;
          break;
        case 'O':
          processval(); valptr=&octave;
          octave=0;
          break;
        case 'T':
          processval(); valptr=&tempo;
          tempo=0;
          break;
        case 'U':
          processval(); valptr=NULL;
          break;
        case 'V':
          processval(); valptr=&volume;
          volume=0;
          break;
        case 'W':
          processval(); valptr=&zzz;
          break;
        case 'X':
          processval(); valptr=&zzz;
          break;
        case 'Y':
          processval(); valptr=&midichan;
          midichan=0;
          break;
        case 'Z':
          processval(); valptr=&midibyte;
          midibyte=0;
          break;
        default:
          error(me,"Unknown char found - 0x%02x",inchr);
          break;
        }
    else
      if(inchr=='!') ignoresw=0;
      else if(inchr=='@') {chrloop+=dospecial();ignoresw=0;}
    }
  WriteVarLen(delta);
  putmidi(0xff2f00,3);
  delta=0;
  }


/***************************************************************************/


void processval() {
  char me[]="processval";

  if((valptr==&duration)&&((duration<1)||(duration>12)))
    error(me,"Invalid duration - %d",duration);
  if((valptr==&midichan)&&((midichan<1)||(midichan>16)))
    error(me,"Invalid MIDI channel - %d",midichan);
  if((valptr==&octave)&&((octave<0)||(octave>8)))
    error(me,"Invalid octave - %d",octave);
  if((valptr==&volume)&&((volume<0)||(volume>15)))
    error(me,"Invalid volume - %d",volume);

  if(valptr==&midibyte) {
    if((midibyte<0)||(midibyte>255))
      midibyte%=256;
/*
      error(me,"Invalid MIDI byte",midibyte);
*/
    if(midibyte>=0x80) {	/* This is MAJOR kludge in that we don't */
      WriteVarLen(delta);	/* _really_ know if we need a delta time */
      delta=0;			/* here or not -- we'll just assume that */
      }				/* bytes >= 0x80 start events...   :-|   */
    putmidi(midibyte,1);
/*
    if(midibyte>=0x80) puts("");
    printf("Z%d=0x%02x\n",midibyte,midibyte);
*/
    }

  if(valptr==&tempo) {
    if((tempo<60)||(tempo>240))
      error(me,"Invalid tempo - %d",tempo);
    WriteVarLen(delta);
    putmidi(0xff5103,3);
    putmidi(60000000/tempo,3);
    delta=0;
    }
  }


int dospecial() {
  char inchr,buffer[200],*buffptr=buffer,me[]="dospecial";
  int sf=0,keytype=0,ok=0,timsigN,timsigD=-1,timsigDD,dsbytes=1;
  inchr=fgetc(ifp);
  if(strindex(".CKNT",inchr)) {
    for(;;) {
      *buffptr=fgetc(ifp);
      dsbytes++;
      if(*buffptr++=='!') {
        *(buffptr-1)='\0';
        break;
        }
      }
    buffptr=buffer;
    switch(inchr) {
      case 'C':
      case 'N':
        putmidi((inchr=='C'?0x00ff02:0x00ff03),3);
        putmidi(strlen(buffer),1);
        for(;*buffptr;)
          putmidi(*buffptr++,1);
        break;
      case 'K':
        if((buffer[0]>='0')&&(buffer[0]<='7')) {
          if((sf=strindex(".b-#",buffer[1]))) {
            sf-=2;
            if((keytype=strindex(".Mm",buffer[2]))) {
              keytype--;
              ok=1;
              }
            }
          }
        if(ok) {
          putmidi(0x00ff5902,4);
          putmidi((((buffer[0]-'0')*sf)<<8)|keytype,2);
          }
        else
          error(me,"Invalid key specifier found",ERROR_MAGIC);
        break;
      case 'T':
        for(;;buffptr++) {
          if((*buffptr=='\0')||(*buffptr=='/'))
            break;
          }
        if(*buffptr) {
          *buffptr++='\0';
          timsigN=atoi(buffer);
          timsigDD=atoi(buffptr);
          while(timsigDD) {
            timsigD++;
            timsigDD>>=1;
            }
          putmidi(0x00ff5804,4);
          putmidi(((timsigN<<24)|(timsigD<<16)|0x2408),4);
          }
        else
          error(me,"Invalid time signature found",ERROR_MAGIC);
        break;
      }
    }
  else
    error(me,"Invalid comment special character found",ERROR_MAGIC);
  return(dsbytes);
  }


/***************************************************************************/


void usage() {
  puts("Usage: s128m2stdmidi <name> [-q]");
  puts("\t\t\t\tFiles:\t<name>[A-H].(header|bytes)");
  puts("\t\t\t\t\t<name>.midi");
  exit(1);
  }


void frd(UBYTE *buffer,ULONG length,char *caller) {
  ULONG actual;
  if((actual=fread(buffer,1,length,ifp))!=length)
    error(caller,"Read only %d bytes from input file",actual);
  }


void fwr(UBYTE *buffer,ULONG length,char *caller) {
  ULONG actual;
  if((actual=fwrite(buffer,1,length,ofp))!=length)
    error(caller,"Wrote only %d bytes to output file",actual);
  }


void error(char *where,char *msg,int number) {
  char msgout[80];
  if(number==ERROR_MAGIC)
    printf("ERROR: %s in %s()\n",msg,where);
  else {
    sprintf(msgout,msg,number);
    printf("ERROR: %s in %s()\n",msgout,where);
    }
  if(ifp) fclose(ifp);
  if(ofp) {fclose(ofp);puts("*** Output file may be useless ***");}
  exit(1);
  }


void writeheader(UWORD tracks) {
  ULONG tmpl;
  UWORD tmpw;
  char me[]="writeheader";
  tmpl=idMThd; fwr((UBYTE *)&tmpl,4,me);	/* chunk type */
  tmpl=6; fwr((UBYTE *)&tmpl,4,me);		/* length */
  tmpw=1; fwr((UBYTE *)&tmpw,2,me);		/* format */
  fwr((UBYTE *)&tracks,2,me);			/* ntrks */
  tmpw=DIVISION; fwr((UBYTE *)&tmpw,2,me);	/* division */
  }


void freedata() {
  struct midiblock *nextblock;
  if((currblock=firstblock)) {
    for(;;) {
      nextblock=currblock->next;
      free(currblock);
      if(!(currblock=nextblock)) break;
      }
    }
  firstblock=NULL;
  midiblockptr=MIDIBLOCK_LEN;
  numberofblocks=0;
  }


void putmidi(ULONG data,int size) {
  struct midiblock *newblock;
  switch(size) {
    case 1:
      if(midiblockptr==MIDIBLOCK_LEN) {
        if((newblock=malloc(sizeof(struct midiblock)))==NULL) {
          freedata();
          error("putmidi","Unable to allocate memory",ERROR_MAGIC);
          }
        newblock->next=NULL;
        if(firstblock) {
          currblock->next=newblock;
          currblock=newblock;
          }
        else firstblock=currblock=newblock;
        midiblockptr=0; numberofblocks++;
        }
      currblock->data[midiblockptr++]=data&0xff;
      break;
    case 2:
      putmidi((data&0xff00)>>8,1);
      putmidi(data&0x00ff,1);
      break;
    case 3:
      putmidi((data&0xff0000)>>16,1);
      putmidi(data&0x00ffff,2);
      break;
    case 4:
      putmidi((data&0xffff0000)>>16,2);
      putmidi(data&0x0000ffff,2);
      break;
    }
  }


void writetrack() {
  ULONG tmpl;
  struct midiblock *nextblock;
  char me[]="writetrack";
  tmpl=idMTrk;
    fwr((UBYTE *)&tmpl,4,me);
  tmpl=MIDIBLOCK_LEN*(numberofblocks-1)+midiblockptr;
    fwr((UBYTE *)&tmpl,4,me);
  if((currblock=firstblock)) {
    for(;;) {
      if((nextblock=currblock->next)) {
        fwr(currblock->data,MIDIBLOCK_LEN,me);
        currblock=nextblock;
        }
      else {
        fwr(currblock->data,midiblockptr,me);
        break;
        }
      }
    }
  }


void WriteVarLen(ULONG value) {
  ULONG buffer;
  buffer=value&0x7f;
  while((value>>=7)>0) {
    buffer<<=8;
    buffer|=0x80;
    buffer+=(value&0x7f);
    }
  while(1) {
    putmidi(buffer,1);
    if(buffer&0x80) buffer>>=8;
    else break;
    }
  }


int strindex(char *s,char c) {
  int loop=0;
  for(;;loop++) {
    if(*s==c) return(loop);
    if(*s++==NULL) return(0);
    }
  }
