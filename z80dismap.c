#include <stdio.h>
#include <stdlib.h>

int lenmap[256] =
{
 1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, // 0x
-2, 3, 1, 1, 1, 1, 2, 1,-2, 1, 1, 1, 1, 1, 2, 1, // 1x
-2, 3, 3, 1, 1, 1, 2, 1,-2, 1, 3, 1, 1, 1, 2, 1, // 2x
-2, 3, 3, 1, 1, 1, 2, 1,-2, 1, 3, 1, 1, 1, 2, 1, // 3x
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4x
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5x
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6x
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7x
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 8x
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 9x
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // Ax
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // Bx
 1, 1,-3,-3,-3, 1, 2, 1, 1, 1,-3, 0,-3,-3, 2, 1, // Cx
 1, 1,-3, 2,-3, 1, 2, 1, 1, 1,-3, 2,-3, 0, 2, 1, // Dx
 1, 1,-3, 1,-3, 1, 2, 1, 1, 1,-3, 1,-3, 0, 2, 1, // Ex
 1, 1,-3, 1,-3, 1, 2, 1, 1, 1,-3, 1,-3, 0, 2, 1  // Fx
};

unsigned char len_ed4[] =
{
  0x43, 0x4b, 0x53, 0x5b, 0x63, 0x6b, 0x73, 0x7b
};

unsigned char len_ddfdcb3[] =
{
  0x34, 0x35, 0x36, 0x46, 0x4E, 0x56, 0x5E, 0x66, 0x6E,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x77, 0x7E,
  0x86, 0x8E, 0x96, 0x9E, 0xA6, 0xAE, 0xB6, 0xBE
};

unsigned char call_opcodes[] =
{
  0xC4, 0xCC, 0xCD, 0xD4, 0xDC, 0xE4, 0xEC, 0xF4, 0xFC
};

unsigned char mem[65536];
int map[65536+4];

// usage of these forms bitfield
// bit 0 (+1) set for code
// bit 1 (+2) set for start of code sub
// bit 2 (+4) set indicates JP (HL)
#define MAP_UNKNOWN 0
#define MAP_CODE    1
#define MAP_CODESUB 3
#define MAP_JP_HL   4

int isin (unsigned char o, unsigned char *l)
{
  int n;
  for (n = 0; l[n] > 0; n++)
    if (o == l[n])
      return 1;
  return 0;
}

void codebranch (int startpc, int sub)
{
  int pc = startpc;
  int len, addlen = 0;
  int setmap = sub != 0 ? MAP_CODESUB : MAP_CODE;
  for (;;)
  {
    if (map[pc] != MAP_UNKNOWN)
      return;
    if (pc > 65536)
      return;
    map[pc] = setmap;
    setmap = MAP_CODE;
    len = 0;
    if (mem[pc] == 0xcb)
      len = 2;
    else if (mem[pc] == 0xed)
    {
      len = 2;
      if (isin (mem[pc+1], len_ed4))
        len = 4;
    }
    else if ( (mem[pc] == 0xdd) || (mem[pc] == 0xfd) )
    {
      if ( (mem[pc+1] == 0xdd) || (mem[pc+1] == 0xfd) )
      {
        pc++;
        addlen++;
        continue;
      }
      if (mem[pc+1] == 0xcb)
        len = 4;
      else if (isin (mem[pc+1], len_ddfdcb3))
        len = 3;
      else
        len = 2;
      len += addlen;
    }
    else
      len = lenmap[mem[pc]];
    addlen = 0;
    if (mem[pc] == 0xe9)
      map[pc] += MAP_JP_HL;
    if (len == 2)
      map[pc+1] = MAP_CODE;
    if (len == 3)
      map[pc+1] = map[pc+2] = MAP_CODE;
    if (len == 4)
      map[pc+1] = map[pc+2] = map[pc+3] = MAP_CODE;
    if (len < 0)
    {
      if (len == -2)
      {
        int newpc;
        map[pc+1] = MAP_CODE;
        newpc = pc + 2;
        if (mem[pc+1] >= 0x80)
          newpc -= (int)(256 - mem[pc+1]);
        else
          newpc += (int)mem[pc+1];
        codebranch (newpc, 0);
      }
      if (len == -3)
      {
        int newpc;
        map[pc+1] = map[pc+2] = MAP_CODE;
        newpc = ((int)mem[pc+2]) << 8;
        newpc += (int)mem[pc+1];
        if (isin (mem[pc], call_opcodes))
        {
          codebranch (newpc, 1);
        }
        else
        {
          codebranch (newpc, 0);
        }
      }
    }
    if (mem[pc] == 0xc9)
      return;
    if (mem[pc] == 0xed)
      if ( (mem[pc] == 0x45) || (mem[pc] == 0x4d) )
        return;
    if (len > 0)
      pc += len;
    else
      pc -= len;
  }
}

void show1map (int maptype)
{
  if ((maptype & 1) == 0)
    puts (" UNKNOWN/DATA");
  else
  {
    printf (" CODE");
    if (maptype & 2)
      printf (" SUB");
    if (maptype & 4)
      printf (" JP (HL)");
    putchar ('\n');
  }
}

void showmap ()
{
  int maptype = map[0];
  int s = 0, o;
  for (o = 0; o < 65536; o++)
  {
    if (map[o] != maptype)
    {
      printf ("%04X-%04X", s, o - 1);
      show1map (maptype);
      s = o;
      maptype = map[o];
    }
  }
  printf ("%04X-FFFF", s);
  show1map (maptype);
}

int main (int argc, char *argv[])
{
  FILE *f;
  if (argc != 3)
  {
    puts ("Usage: z80dismap <memfile> <startpc>");
    return 0;
  }
  f = fopen (argv[1], "rb");
  if (f == NULL)
  {
    puts ("File error");
    return 0;
  }
  fread (mem, 1, 65536, f);
  fclose (f);
  codebranch (atoi (argv[2]), 1);
  showmap ();
  return 0;
}

