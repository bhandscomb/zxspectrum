# zxspectrum
ZX Spectrum related experimentations and investigations

Most of these are VERY old - for example use K&R style syntax due to some really old Amiga C compilers used in the 90s, very bad formatting, and sometimes designed for ZX files being in a now weird split "header" and "bytes" format. This was a format that was spit out of a certain ZX Spectrum emulator on the Amiga created by Peter McGavin (think it was just called "Spectrum 1.7")

imager
* ZX Spectrum tool written using HiSoft C to perform some simple maniputation of SCREEN$ saved images - only for display, did not have ability to save.

ami2zxhsc
* Written on Amiga to convert text style C code to HiSoft C on ZX Spectrum.

zxhsc2ami
* Written on Amiga to convert HiSoft C code from ZX Spectrum to text file.

tas2conv
* Takes Tasword 2 file and does a very basic conversion to text format (leaves all lines 64 characters long)

s128m2stdmidi
* Used to parse saved ZX 128K mode PLAY strings designed for MIDI and create SMF MIDI files.

zeusprint
* Read source file from Crystal Computing ZEUS assembler and produced plain text output.

zeus-format.txt
* Documentation of ZEUS source format gleaned during the above decoder. Pure deduction, no reverse engineering of ZEUS code was performed.

dumpfm.c
* Experimental tool to "dump" data held by OCP Finance Manager.

z80tomem
* Takes a "z80" format snapshot and a ZX ROM image file and makes a 64k image of memory.

z80dismap
* Experiment to take a 64k image of memory (see z80tomem) and a start program counter and do a very simple "trace" to identify code and data regions.

getser
* Created on an Amiga to receive SAVEs from ZX Interface 1, creating a "header" and "bytes" file.

zxflib
* Attempt to make a "library" to more easily use the header/bytes files format.

baslist.c
* Used to take a ZX BASIC program (bytes/header) and write it out in plain text. Written to use zxflib.

blocks2tzx
* This is a simple hack to make a TZX usable in a modern emulator from files created by getser.

uspeech
* After seeing a Facebook posting I was inspired to create a disassembly of the Currah Microspeech ROM, by hand.

mdrinfo
* Rough program to "map" Microdrive cartridge file, also showing cart name and a simple catalog

mdr2bas
* Rough program to extract BASIC programs from Microdrive cartridge file, saving as ".bas" PLUS3DOS format (e.g. can load onto ZX Spectrum Next).
