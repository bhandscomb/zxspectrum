
/* zxhsc2ami.c */

#include <stdio.h>
#include <stdlib.h>

FILE *ifp=NULL,*ofp=NULL;

void error(char *);

main(argc,argv)
  int argc;
  char *argv[];
  {
  char name[80];
  unsigned char zfile[516],parity;
  int block,parloop,blocklen,done=0;

  if(argc!=2)
    error("args - Usage: zxhsc2ami <zname>");
  if((ofp=fopen(argv[1],"wb"))==NULL)
    error("open outfile");
  for(block=1;!done;block++) {
    sprintf(name,"%s.bytes%02d",argv[1],block);
    if((ifp=fopen(name,"rb"))==NULL)
      error("open infile");
    if(fread(zfile,sizeof(char),516,ifp)!=516)
      error("read infile");
    fclose(ifp); ifp=NULL;
    parity=0;
    for(parloop=0;parloop<515;parloop++) parity^=zfile[parloop];
    if(parity!=zfile[515])
      error("parity infile");
    blocklen=(((zfile[2]&0x7f)<<8)+zfile[1]);
    if(fwrite(&zfile[3],sizeof(char),blocklen,ofp)!=blocklen)
      error("write outfile");
    done=zfile[2]&0x80;
    }
  fclose(ofp); ofp=NULL;
  }

void error(msg)
  char *msg;
  {
  printf("ERROR - %s\n",msg);
  if(ifp) fclose(ifp);
  if(ofp) fclose(ofp);
  exit(1);
  }
