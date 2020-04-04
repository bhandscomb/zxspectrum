/* mdrinfo.c */

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
  // unsigned char raw1[15];
  // unsigned char raw2[15];
  struct cat *catnext;
};

struct cat *catfirst, *catlast;

void addcat (unsigned char *h1, unsigned char *h2, unsigned char *d)
{
  struct cat *catnew;
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
  // memcpy (catnew->raw1, h1, 15);
  // memcpy (catnew->raw2, h2, 15);
}

void showcat (void)
{
  struct cat *e;
  e = catfirst;
  while (e != NULL)
  {
    // for (int n = 0; n < 15; n++) printf ("%02x ", e->raw1[n]); putchar ('\n');
    // for (int n = 0; n < 15; n++) printf ("%02x ", e->raw2[n]); putchar ('\n');
    switch (e->type)
    {
      case 0:
        printf ("BASIC \"%s\" ", e->name);
        if (e->line != 65535)
        {
          printf ("LINE %d ", e->line);
        }
        printf
        (
          "(BAS=%d VAR=%d TOT=%d)\n",
          e->lenbas, e->len - e->lenbas, e->len
        );
        break;
      case 1: case 2: printf ("ARRAY \"%s\" \n", e->name); break;
      case 3:
        printf
        (
          "BYTES \"%s\",%d,%d\n",
          e->name,
          e->addr, e->len
        );
        break;
      default: printf ("????? \"%s\"\n", e->name);
    }
    catfirst = e->catnext;
    free (e);
    e = catfirst;
  }
}

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

void mdrinfo (FILE *f)
{
  char mdrname[11];
  char fname[11];
  unsigned char hdr1[15];
  unsigned char hdr2[15];
  unsigned char dat[512 + 1];
  mdrname[10] = fname[10] = '\0';
  catfirst = catlast = NULL;
  int c = 0;
  for (int n = 0; n < 254; n++)
  {
    // header 1, sector
    fread (hdr1, 1, 15, f);
    if ((hdr1[0] & 1) != 1)
    {
      printf ("BADHDR-HDFLAG  ");
      fseek (f, 15 + 512 + 1, SEEK_CUR);
      goto next;
    }
    if (hdr1[14] != chk (hdr1, 14))
    {
      printf ("BADHDR-HDCHK   ");
      fseek (f, 15 + 512 + 1, SEEK_CUR);
      goto next;
    }
    memcpy (mdrname, hdr1 + 4, 10);
    // header 2, record
    fread (hdr2, 1, 15, f);
    if ((hdr2[0] & 1) != 0)
    {
      printf ("BADHDR-RECFLG  ");
      fseek (f, 512 + 1, SEEK_CUR);
      goto next;
    }
    if (hdr2[14] != chk (hdr2, 14))
    {
      printf ("BADREC-DESCHK  ");
      fseek (f, 512 + 1, SEEK_CUR);
      goto next;
    }
    if ((hdr2[2] == 0) && (hdr2[3] == 0))
    {
      printf ("-------------  ");
      fseek (f, 512 + 1, SEEK_CUR);
      goto next;
    }
    memcpy (fname, hdr2 + 4, 10);
    printf ("%10s:%02x", fname, (int) hdr2[1]);
    fread (dat, 1, 512 + 1, f);
    if (dat[512] != chk (dat, 512))
    {
      printf ("! ");
    }
    else
    {
      printf ("  ");
      if (hdr2[1] == 0)
      {
        addcat (hdr1, hdr2, dat);
      }
    }
next:
    c++;
    if (c == 4)
    {
      putchar ('\n');
      c = 0;
    }
  }
  putchar ('\n');
  printf ("MDR:%s\n", mdrname);
  showcat ();
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
  mdrinfo (f);
  fclose (f);
  return 0;
}

