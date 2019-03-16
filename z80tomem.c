#include <stdio.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
  unsigned char z80[65536];
  unsigned char mem[65536];
  FILE *f;
  size_t fs;
  int filever = 0;
  int hdrsize = 0;
  if (argc!=4)
  {
    puts ("Usage: z80tomem <z80file> <romfile> <memfile>");
    return 0;
  }
  f = fopen(argv[2],"rb");
  if (f==NULL)
  {
    puts ("ROM file error");
    return 0;
  }
  fseek (f,0,SEEK_END);
  fs = ftell (f);
  fseek (f,0,SEEK_SET);
  if (fs != 0x4000)
  {
    fclose (f);
    puts ("Bad ROM");
    return 0;
  }
  fread (mem,1,0x4000,f);
  fclose (f);
  f = fopen(argv[1],"rb");
  if (f==NULL)
  {
    puts ("Input file error");
    return 0;
  }
  fseek (f,0,SEEK_END);
  fs = ftell (f);
  fseek (f,0,SEEK_SET);
  fread (z80,1,fs,f);
  fclose (f);
  if (fs < 100)
  {
    puts ("Huh?");
    return 0;
  }
  if ((z80[6]>0)||(z80[7]>0))
    filever=1;
  else if (z80[30]==23)
    filever=2;
  else if (z80[30]==54)
    filever=3;
  else if (z80[30]==55)
    filever=3;
  if (filever==0)
  {
    printf("Unknown snapshot version (2nd header %d bytes)",z80[30]);
    return 0;
  }
  hdrsize=30+(filever>1?2+z80[30]:0);
  printf("Snapshot version %d (%d/0x%x header bytes)\n",filever,hdrsize,hdrsize);
  if (filever==1)
  {
    puts("not doing filever 1");
  }
  else
  {
    int o=hdrsize;
    while(o<fs)
    {
      int m;
      printf("%04x Page %d compressed %02x%02x\n",o,z80[o+2],z80[o+1],z80[o]);
      if(z80[o+2]==8)
        m=0x4000;
      else if(z80[o+2]==4)
        m=0x8000;
      else if(z80[o+2]==5)
        m=0xc000;
      else
        m=0;
      if (m!=0)
      {
        int c;
        unsigned char *s,*d;
        s=&z80[o+3];
        d=&mem[m];
        c=z80[o]+256*z80[o+1];
        while(c>0)
        {
          if((s[0]==0xed)&&(s[1]==0xed))
          {
            int z;
            for (z=0;z<s[2];z++)
              *d++=s[3];
            s+=4;
            c-=4;
          }
          else
          {
            *d++=*s++;
            c--;
          }
        }
      }
      o+=3+z80[o]+256*z80[o+1];
    }
    f=fopen(argv[3],"wb");
    if (f)
    {
      fwrite(mem,1,65536,f);
      fclose(f);
    }
  }
  return 0;
}

