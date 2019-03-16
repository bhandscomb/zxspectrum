/* getser.c */

#include <stdio.h>
#include <stdlib.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <devices/serial.h>

#include <clib/exec_protos.h>
#include <clib/alib_protos.h>

#define BAUDRATE 300

__far UBYTE ZXbuffer[66000];

UBYTE x;

int y;

UBYTE hdr[19]={0,0,32,32,32,32,32,32,32,32,32,32,0,0,0,0,0,0,0};

char fn[20];

int main(int argc,char *argv[]) {
  struct MsgPort *serMP;
  struct IOExtSer *serIO;
  FILE *fp;
  if(argc!=2) {
    puts("ERROR - Invalid Args\nUsage: getser <filename>");
    exit(1);
    }
  strncpy(&hdr[2],argv[1],strlen(argv[1]));
  if(serMP=CreateMsgPort()) {
    if(serIO=CreateIORequest(serMP,sizeof(struct IOExtSer))) {
      serIO->io_SerFlags=SERF_SHARED;
      if(OpenDevice("serial.device",0,(struct IORequest *)serIO,0))
        puts("ERROR - serial.device did not open");
      else {
        serIO->IOSer.io_Command=SDCMD_SETPARAMS;
        serIO->io_SerFlags&=~SERF_PARTY_ON;
        serIO->io_SerFlags|=SERF_XDISABLED|SERF_RAD_BOOGIE;
        serIO->io_Baud=BAUDRATE;
        if(DoIO((struct IORequest *)serIO))
          puts("ERROR - Unable to set parameters");
        else {
          serIO->IOSer.io_Command=CMD_READ;
          serIO->IOSer.io_Data=ZXbuffer;
          serIO->IOSer.io_Length=9;
          DoIO((struct IORequest *)serIO);
          if((ZXbuffer[0]==0)||(ZXbuffer[0]==3)) {
            serIO->IOSer.io_Data=&ZXbuffer[9];
            serIO->IOSer.io_Length=ZXbuffer[2]*256+ZXbuffer[1];
            DoIO((struct IORequest *)serIO);
            hdr[12]=ZXbuffer[1];
            hdr[13]=ZXbuffer[2];
            hdr[1]=ZXbuffer[0];
            if(ZXbuffer[0]==0) {
              hdr[14]=128;
              hdr[15]=128;
              hdr[16]=ZXbuffer[5];
              hdr[17]=ZXbuffer[6];
              }
            else {
              hdr[14]=ZXbuffer[3];
              hdr[15]=ZXbuffer[4];
              }
            x=0;
            for(y=0;y<18;y++) x^=hdr[y];
            hdr[18]=x;
            sprintf(fn,"%s.header",argv[1]);
            if(fp=fopen(fn,"wb")) {
              fwrite(hdr,19,1,fp);
              fclose(fp);
              }
            x=255;
            for(y=0;y<ZXbuffer[2]*256+ZXbuffer[1];y++) x^=ZXbuffer[y+9];
            sprintf(fn,"%s.bytes",argv[1]);
            if(fp=fopen(fn,"wb")) {
              fputc(255,fp);
              fwrite(&ZXbuffer[9],ZXbuffer[2]*256+ZXbuffer[1],1,fp);
              fputc(x,fp);
              fclose(fp);
              }
            }
          }
        CloseDevice((struct IORequest *)serIO);
        }
      DeleteIORequest(serIO);
      }
    else
      puts("ERROR - Unable to create IO request");
    DeleteMsgPort(serMP);
    }
  else
    puts("ERROR - Unable to create message port");
  return 0;
  }
