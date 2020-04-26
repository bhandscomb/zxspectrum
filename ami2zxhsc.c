
/* ami2zxhsc.c */

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
  unsigned char zhdr[19]={0,3,32,32,32,32,32,32,32,32,32,32,0,0,0,0,0,0,0};

  if(argc!=2)
    error("args - Usage: ami2zxhsc <zname>");
  if((ifp=fopen(argv[1],"rb"))==NULL)
    error("open infile");
  zfile[0]=0xff;
  for(block=1;!done;block++) {
    sprintf(name,"%s.bytes%02d",argv[1],block);
    if((ofp=fopen(name,"wb"))==NULL)
      error("open outfile");
    blocklen=fread(&zfile[3],sizeof(char),512,ifp);
    zfile[2]=(blocklen/256)|(blocklen==512?0x00:0x80);
    zfile[1]=blocklen%256;
    parity=0;
    for(parloop=0;parloop<515;parloop++) parity^=zfile[parloop];
    zfile[515]=parity;
    if(fwrite(zfile,sizeof(char),516,ofp)!=516)
      error("write outfile");
    fclose(ofp); ofp=NULL;
    done=zfile[2]&0x80;
    }
  fclose(ifp); ifp=NULL;
  strncpy(&zhdr[2],argv[1],strlen(argv[1]));
  parity=0;
  for(parloop=0;parloop<18;parloop++) parity^=zhdr[parloop];
  zhdr[18]=parity;
  sprintf(name,"%s.header",argv[1]);
  if((ofp=fopen(name,"wb"))==NULL)
    error("open outfile");
  if(fwrite(zhdr,sizeof(char),19,ofp)!=19)
    error("write outfile");
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
