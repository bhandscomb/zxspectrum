/* mdr2bas.c */
/* mdrinfo stripped down and extraction added */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct cat
{
  char name[11];
  int type;
  int addr;
  int len;
  int lenbas;
  int line;
  struct cat *catnext;
};

struct cat *catfirst, *catlast;

unsigned char chk (unsigned char *s, int l)
{
  int c = 0;
  unsigned char *p = s;
  for (int n = 0; n < l; n++)
  {
    c += *p++;
    if (c > 254)
      c -= 255;
  }
  return c;
}

void addcat (unsigned char *h1, unsigned char *h2, unsigned char *d)
{
  struct cat *e, *catnew;
  // strangely need to do this...
  e = catfirst;
  while (e != NULL)
  {
    if (memcmp (e->name, h2 + 4, 10) == 0)
    {
      return;
    }
    e = e->catnext;
  }
  // now get on with adding
  catnew = malloc (sizeof (struct cat));
  if (catnew == NULL)
  {
    return;
  }
  if (catfirst == NULL)
  {
    catfirst = catlast = catnew;
  }
  else
  {
    catlast->catnext = catnew;
    catlast = catnew;
  }
  catnew->catnext = NULL;
  memcpy (catnew->name, h2 + 4, 10);
  catnew->name[10] = '\0';
  catnew->type = (int)d[0];
  catnew->addr = (((int)d[4]) << 8) | (int)d[3];
  catnew->len = (((int)d[2]) << 8) | (int)d[1];
  catnew->lenbas = (((int)d[6]) << 8) | (int)d[5];
  catnew->line = (((int)d[8]) << 8) | (int)d[7];
}

void writefiles (FILE *f)
{
  struct cat *e;
  unsigned char hdr[15];
  unsigned char dat[513];
  unsigned char buff[256 * 512];
  char oname[20];
  e = catfirst;
  while (e != NULL)
  {
    // pull all blocks (single pass, in whatever order they appear within mdr)
    rewind (f);
    for (int n = 0; n < 254; n++)
    {
      fread (hdr, 1, 15, f);
      if ((hdr[0]&1) != 1) { fseek(f,15+512+1,SEEK_CUR); continue; }
      if (hdr[14] != chk(hdr,14)) { fseek(f,15+512+1,SEEK_CUR); continue; }
      fread (hdr, 1, 15, f);
      if ((hdr[0]&1) != 0) { fseek(f,512+1,SEEK_CUR); continue; }
      if (hdr[14] != chk(hdr,14)) { fseek(f,512+1,SEEK_CUR); continue; }
      if ((hdr[2]==0) && (hdr[3]==0)) { fseek(f,512+1,SEEK_CUR); continue; }
      fread (dat, 1, 512+1, f);
      if (dat[512] == chk(dat,512))
      {
        if (memcmp (e->name, hdr + 4, 10) == 0)
        {
          memcpy (buff + (((int) hdr[1]) << 9), dat, 512);
        }
      }
    }
    // safety first (ultrasafe)
    for (int n = 0; n < 10; n++)
    {
      if
      (
        (e->name[n] < 0x20) ||
        (e->name[n] > 0x7e) ||
        (e->name[n] == '?') ||
        (e->name[n] == '*') ||
        (e->name[n] == '/') ||
        (e->name[n] == '\\') ||
        (e->name[n] == ':') ||
        (e->name[n] == '<') ||
        (e->name[n] == '>') ||
        (e->name[n] == '|') ||
        (e->name[n] == '\"') ||
        (e->name[n] == ',') ||
        (e->name[n] == '+') ||
        (e->name[n] == ';') ||
        (e->name[n] == '[') ||
        (e->name[n] == ']') ||
        (e->name[n] == '=')
      )
      {
        e->name[n] = '_';
      }
    }
    // rtrim name
    for (int n = 9; n > 0; n--)
    {
      if (e->name[n] == ' ')
      {
        e->name[n] = '\0';
      }
      else
      {
        break;
      }
    }
    if (strlen (e->name) == 0)
    {
      strcpy (e->name, "_spaces_");
    }
    if (e->type == 0)
    {
      FILE *o;
      unsigned char bashdr[128];
      for (int n = 0; n < 128; n++) bashdr[n] = 0x00;
      strcpy ((char *)bashdr, "PLUS3DOS");
      bashdr[8] = 0x1a; bashdr[9] = 0x01; bashdr[10] = 0x00;
      int flen = 128 + e->len;
      bashdr[11] = flen & 0xff;
      bashdr[12] = flen >> 8;
      bashdr[16] = e->len & 0xff;
      bashdr[17] = e->len >> 8;
      bashdr[19] = 0x80; // we don't want these to autorun
      bashdr[20] = e->lenbas & 0xff;
      bashdr[21] = e->lenbas >> 8;
      unsigned char c = 0;
      for (int n = 0; n < 127; n++) c += bashdr[n];
      bashdr[127] = c;
      sprintf (oname, "%s.bas", e->name);
      o = fopen (oname, "wb");
      if (o != NULL)
      {
        fwrite (bashdr, 1, 128, o);
        fwrite (buff + 9, 1, e->len, o);
        fclose (o);
      }
    }
    catfirst = e->catnext;
    free (e);
    e = catfirst;
  }
}

void mdrinfo (FILE *f)
{
  char mdrname[11];
  char fname[11];
  unsigned char hdr1[15];
  unsigned char hdr2[15];
  unsigned char dat[512 + 1];
  mdrname[10] = fname[10] = '\0';
  catfirst = catlast = NULL;
  for (int n = 0; n < 254; n++)
  {
    // header 1, sector
    fread (hdr1, 1, 15, f);
    if ((hdr1[0] & 1) != 1)
    {
      fseek (f, 15 + 512 + 1, SEEK_CUR);
      continue;
    }
    if (hdr1[14] != chk (hdr1, 14))
    {
      fseek (f, 15 + 512 + 1, SEEK_CUR);
      continue;
    }
    memcpy (mdrname, hdr1 + 4, 10);
    // header 2, record
    fread (hdr2, 1, 15, f);
    if ((hdr2[0] & 1) != 0)
    {
      fseek (f, 512 + 1, SEEK_CUR);
      continue;
    }
    if (hdr2[14] != chk (hdr2, 14))
    {
      fseek (f, 512 + 1, SEEK_CUR);
      continue;
    }
    if ((hdr2[2] == 0) && (hdr2[3] == 0))
    {
      fseek (f, 512 + 1, SEEK_CUR);
      continue;
    }
    memcpy (fname, hdr2 + 4, 10);
    fread (dat, 1, 512 + 1, f);
    if (dat[512] == chk (dat, 512))
    {
      if (hdr2[1] == 0)
      {
        addcat (hdr1, hdr2, dat);
      }
    }
  }
}

int main (int argc, char *argv[])
{
  FILE *f;
  if (argc != 2)
  {
    puts ("arg error");
    return -1;
  }
  f = fopen (argv[1], "rb");
  if (f == NULL)
  {
    puts ("fopen error");
    return -1;
  }
  // core of mdrinfo used to catalog "cartridge"
  mdrinfo (f);
  writefiles (f);
  fclose (f);
  return 0;
}

