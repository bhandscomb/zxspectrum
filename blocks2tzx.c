// boc2tzx.c

#include <stdio.h>
#include <stdlib.h>

unsigned char tzxhdr[] =
{
  0x5a,0x58,0x54,0x61,0x70,0x65,0x21,0x1a,
  0x01,0x0a
};

unsigned char buffer[65536];

int main (int argc, char *argv[])
{
  char fn[200];
  FILE *i, *o;
  long s;
  if (argc != 2)
    return 0;
  sprintf (fn, "%s.tzx", argv[1]);
  o = fopen (fn, "wb");
  if (o)
  {
    fwrite (tzxhdr, 10, 1, o);
    sprintf (fn, "%s.header", argv[1]);
    i = fopen (fn, "rb");
    if (i)
    {
      fseek (i, 0, SEEK_END);
      s = ftell (i);
      fseek (i, 0, SEEK_SET);
      if (s < 65536)
      {
        buffer[0] = 0x10;
        buffer[1] = 0xe8; buffer[2] = 0x03;
        buffer[3] = s & 0xff; buffer[4] = s >> 8;
        fwrite (buffer, 5, 1, o);
        fread (buffer, s, 1, i);
        fwrite (buffer, s, 1, o);
      }
      fclose (i);
      sprintf (fn, "%s.bytes", argv[1]);
      i = fopen (fn, "rb");
      if (i)
      {
        fseek (i, 0, SEEK_END);
        s = ftell (i);
        fseek (i, 0, SEEK_SET);
        if (s < 65536)
        {
          buffer[0] = 0x10;
          buffer[1] = 0x00; buffer[2] = 0x10;
          buffer[3] = s & 0xff; buffer[4] = s >> 8;
          fwrite (buffer, 5, 1, o);
          fread (buffer, s, 1, i);
          fwrite (buffer, s, 1, o);
        }
        fclose (i);
      }
    }
    fclose (o);
  }
  return 0;
}
