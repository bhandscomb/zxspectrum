/* tas2conv.c */

#include <stdio.h>
#include <stdlib.h>

void error(char *);

FILE *ifp=NULL,*ofp=NULL;

main(argc,argv)
  int argc;
  char *argv[];
  {
  char linebuffer[66]; /* 64 + '\n' + evenpadbyte */

  if(argc!=3)
    error("Usage: tas2conv <infile> <outfile>");
  if((ifp=fopen(argv[1],"rb"))==NULL)
    error("fopen(rb)");
  if((ofp=fopen(argv[2],"wb"))==NULL)
    error("fopen(wb)");
  fgetc(ifp);
  linebuffer[64]='\n';
  linebuffer[65]='\0';
  for(;;) {
    if(fread(linebuffer,1,64,ifp)!=64)
      break;
    linebuffer[64]='\n';
    if(fwrite(linebuffer,1,65,ofp)!=65)
      error("fwrite");
    }
  fclose(ifp);
  fclose(ofp);
  }

void error(msg)
  char *msg;
  {
  if(ifp) fclose(ifp);
  if(ofp) fclose(ofp);
  printf("ERROR - %s\n",msg);
  exit(1);
  }
