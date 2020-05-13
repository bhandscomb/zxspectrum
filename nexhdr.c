/* nexhdr.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char hdr[512];

void out_tag (char *tag)
{
  printf ("%20s: ", tag);
}

void out_str (char *tag, int pos, int len)
{
  out_tag (tag);
  putchar ('\"');
  for (int n = 0; n < len; n++)
  {
    unsigned char c = (char) hdr[pos+n];
    if ((c < 32) || (c > 126))
    {
      c = '.';
    }
    putchar (c);
  }
  putchar ('\"');
}

void out_byte (char *tag, int pos)
{
  int n = (int) hdr[pos];
  out_tag (tag);
  printf ("%d (%02xh)", n, n);
}

void out_word (char *tag, int pos)
{
  int n = (int) hdr[pos];
  n |= ((int) hdr[pos+1]) << 8;
  out_tag (tag);
  printf ("%d (%04xh)", n, n);
}

void out_banks (char *tag, int pos)
{
  int n, b;
  out_tag (tag);
  for (n = 0; n < 28; n++) { b = (int) hdr[pos+0+n]; printf ("%01x ", b); }
  putchar ('\n');
  out_tag ("");
  for (n = 0; n < 28; n++) { b = (int) hdr[pos+28+n]; printf ("%01x ", b); }
  putchar ('\n');
  out_tag ("");
  for (n = 0; n < 28; n++) { b = (int) hdr[pos+56+n]; printf ("%01x ", b); }
  putchar ('\n');
  out_tag ("");
  for (n = 0; n < 28; n++) { b = (int) hdr[pos+84+n]; printf ("%01x ", b); }
}

void out_tri (char *tag, int pos)
{
  int a = (int) hdr[pos];
  int b = (int) hdr[pos+1];
  int c = (int) hdr[pos+2];
  out_tag (tag);
  printf ("%d.%d.%d", a, b, c);
}

void out_dword (char *tag, int pos)
{
  out_tag (tag);
  printf ("%02X", hdr[pos+3]);
  printf ("%02X", hdr[pos+2]);
  printf ("%02X", hdr[pos+1]);
  printf ("%02X", hdr[pos]);
}

void out_tileregs (char *tag, int pos)
{
  out_tag (tag);
  printf ("6B=%02X ", hdr[pos]);
  printf ("6C=%02X ", hdr[pos+1]);
  printf ("6E=%02X ", hdr[pos+2]);
  printf ("6F=%02X ", hdr[pos+3]);
}

void dumphdr (void)
{
  out_str ("Signature", 0, 4); putchar ('\n');
  out_str ("NEX format", 4, 4); putchar ('\n');
  out_byte ("RAM Reqd", 8); puts (" 0=768K 1=1792K");
  out_byte ("Num Banks In File", 9); putchar ('\n');
  out_byte ("Loading Scr Flags", 10); puts (" NoPal Flags2 X HiCol HiRes LoRes ULA Layer2");
  out_byte ("Border Colour", 11); putchar ('\n');
  out_word ("Stack Pointer", 12); putchar ('\n');
  out_word ("Program Counter", 14); puts (" 0 = just load");
  out_word ("Num Extra Files", 16); putchar ('\n');
  out_banks ("Bank Flag", 18); putchar ('\n');
  out_byte ("L2 Loading Bar", 130); putchar ('\n');
  out_byte ("Loading Bar Clr", 131); putchar ('\n');
  out_byte ("Bank Load Delay", 132); putchar ('\n');
  out_byte ("Start Delay", 133); putchar ('\n');
  out_byte ("Preserve NextRegs", 134); putchar ('\n');
  out_tri ("Required Core", 135); putchar ('\n');
  out_byte ("HiRes Colour", 138); puts (" per port 255");
  out_byte ("Entry bank CDEF", 139); putchar ('\n');
  out_word ("File Handle Addr", 140); putchar ('\n');
  out_byte ("Expansion Bus", 142); putchar ('\n');
  out_byte ("Checksum?", 143); putchar ('\n');
  out_dword ("File Offset", 144); putchar ('\n');
  out_word ("CLI Buff Addr", 148); putchar ('\n');
  out_word ("CLI Buff Size", 150); putchar ('\n');
  out_byte ("Load Scr Flags2", 152); putchar ('\n');
  out_byte ("Copper Code Block?", 153); putchar ('\n');
  out_tileregs ("Tilemode Regs", 154); putchar ('\n');
  out_byte ("L2 Load Bar Y", 158); putchar ('\n');
}

int main (int argc, char *argv[])
{
  FILE *f;
  if (argc != 2)
  {
    puts ("Usage: nexhdr <file>");
    return 0;
  }
  f = fopen (argv[1], "rb");
  if (f == NULL)
  {
    puts ("Unable to open file");
    return 0;
  }
  if (fread (hdr, 1, 512, f) != 512)
  {
    puts ("Couldn't read 512 bytes");
    fclose (f);
    return 0;
  }
  fclose (f);
  if (strncmp (hdr, "Next", 4) != 0)
  {
    puts ("Missing \"Next\" signature");
    return 0;
  }
  out_tag ("Filename");
  puts (argv[1]);
  dumphdr ();
  return 0;
}

