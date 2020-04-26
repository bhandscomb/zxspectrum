

/**********************/

/*       IMAGER       */

/* (C) HANDSCOMB-SOFT */

/*      Vers 1.5      */

/*      30/03/89      */

/*                    */

/*     Written in     */

/*      HiSoft C      */

/**********************/



#define flip 1
#define invert 2
#define mirror 3
#define compl 4

#define pushregs 0xf5,0xc5,0xd5,0xe5
#define popregs 0xe1,0xd1,0xc1,0xf1
 
typedef char * __char_ptr;

static char buffer[6912];
static char title[11];

main()
  {
  char choice;

  getpic();
  choice=' ';
  while (choice!='x')
    {
    choice=menu();
    switch (choice)
      {
      case 'l':getpic();break;
      case 'd':displ();rawin();break;
      case 'f':image(flip);break;
      case 'i':image(invert);break;
      case 'm':image(mirror);break;
      case 'c':image(compl);break;
      case 'x':goodbye();break;
      }
    }
  }

menu()
  {
  char k;
 
  cls();
  title[10]='\0';
  printf("\n       IMAGER Version 1.5");
  printf("\n\n        Title:%s",title);
  printf("\n\n\n     L   Load new image");
  printf("\n\n     D   Display image");
  printf("\n\n     F   Flip image");
  printf("\n\n     I   Invert image");
  printf("\n\n     M   Mirror image");
  printf("\n\n     C   Complement colours");
  printf("\n\n     X   eXit IMAGER");
  printf("\n\n\n        Choice:");
  k=' ';
  while (k==' ')
    {
    k=tolower(rawin());
    if ((k=='d')||(k=='f')
      ||(k=='i')||(k=='x')
      ||(k=='l')||(k=='m')
      ||(k=='c'))
        putchar(toupper(k));
    else
      k=' ';
    }
  return k;
  }

getpic()
  {
  inline(pushregs,
    0xdd,0xe5,
    0xdd,0x21,&buffer[0],
    0xdd,0xe5,
    0x11,0x11,0x00,
    0xaf,
    0x37,
    0xcd,0x0556,
    0x21,&buffer[1],
    0x11,&title[0],
    0x01,0x0a,0x00,
    0xed,0xb0,
    0xdd,0xe1,
    0x11,0x00,0x1b,
    0x3e,0xff,
    0x37,
    0xcd,0x0556,
    0xdd,0xe1,
    popregs);
  }

displ()
  {
  inline(pushregs,
    0x21,&buffer[0],
    0x11,0x4000,
    0x01,0x1b00,
    0xed,0xb0,
    popregs);
  }

store()
  {
  inline(pushregs,
    0x21,0x4000,
    0x11,&buffer[0],
    0x01,0x1b00,
    0xed,0xb0,
    popregs);
  }

image(func)
  char func;
  {
  displ();
  switch (func)
    {
    case flip:iflip();break;
    case invert:iinvert();break;
    case mirror:imirror();break;
    case compl:icompl();break;
    }
  store();
  }

poke(addr,val)
  {
  * cast(__char_ptr) addr = val;
  }

peek(addr)
  {
  return * cast(__char_ptr) addr;
  }

unsigned findaddr(x,y)
  char x,y;
  {
  unsigned addr;
  char y1,y2,y3,hi,lo;

  y1=(y&0xc0)>>3;
  y2=y&0x07;
  y3=(y&0x38)<<2;
  hi=0x40|y1|y2;
  lo=y3|x;
  addr=hi*256+lo;
  return addr;
  }

iflip()
  {
  char xpos,ypos1,ypos2,byte1,byte2;
  unsigned addr1,addr2;

  for (xpos=0;xpos<32;xpos++)
    {
    for (ypos1=0;ypos1<96;ypos1++)
      {
      ypos2=191-ypos1;
      addr1=findaddr(xpos,ypos1);
      addr2=findaddr(xpos,ypos2);
      byte1=peek(addr1);
      byte2=peek(addr2);
      poke(addr1,byte2);
      poke(addr2,byte1);
      }
    for (ypos1=0;ypos1<12;ypos1++)
      {
      ypos2=23-ypos1;
      addr1=22528+32*ypos1+xpos;
      addr2=22528+32*ypos2+xpos;
      byte1=peek(addr1);
      byte2=peek(addr2);
      poke(addr1,byte2);
      poke(addr2,byte1);
      }
    }
  }

iinvert()
  {
  char xpos,ypos,byte;
  unsigned addr;
 
  for (xpos=0;xpos<32;xpos++)
    for (ypos=0;ypos<192;ypos++)
      {
      addr=findaddr(xpos,ypos);
      byte=peek(addr);
      byte=255-byte;
      poke(addr,byte);
      }
  }

goodbye()
  {
  cls();
  printf("  ##  # # ###\n");
  printf("  # # # # #\n");
  printf("  ##   #  ##\n");
  printf("  # #  #  #\n");
  printf("  ##   #  ###\n");
  }

char mirrorbyte(byte)
  char byte;
  {
  char byte2,loop;

  byte2=0;
  for(loop=0;loop<8;loop++)
    {
    byte2=byte2*2;
    if ((byte%2)==1)
      byte2=byte2+1;
    byte=byte/2;
    }
  return byte2;
  }

imirror()
  {
  char xpos1,xpos2,ypos,byte1,byte2;
  unsigned addr1,addr2;

  for (xpos1=0;xpos1<16;xpos1++)
    {
    xpos2=31-xpos1;
    for (ypos=0;ypos<192;ypos++)
      {
      addr1=findaddr(xpos1,ypos);
      addr2=findaddr(xpos2,ypos);
      byte1=mirrorbyte(peek(addr1));
      byte2=mirrorbyte(peek(addr2));
      poke(addr1,byte2);
      poke(addr2,byte1);
      }
    for (ypos=0;ypos<24;ypos++)
      {
      addr1=22528+32*ypos+xpos1;
      addr2=22528+32*ypos+xpos2;
      byte1=peek(addr1);
      byte2=peek(addr2);
      poke(addr1,byte2);
      poke(addr2,byte1);
      }
    }
  }

icompl()
  {
  unsigned addr;
  char byte;

  for (addr=0;addr<768;addr++)
    {
    byte=peek(addr+22528);
    byte=byte^0x3f;
    poke(addr+22528,byte);
    }
  }

cls()
  {
  inline(0xcd,0x0d6b);
  }
