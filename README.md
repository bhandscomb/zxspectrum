# zxspectrum
ZX Spectrum related experimentations and investigations

uspeech
* after seeing a Facebook posting I was inspired to create a disassembly of the Currah Microspeech ROM, by hand.

z80tomem
* takes a "z80" format snapshot and a ZX ROM image file and makes a 64k image of memory.

z80dismap
* experiment to take a 64k image of memory (see z80tomem) and a start program counter and do a very simple "trace" to identify code and data regions.

getser
* created on an Amiga to receive SAVEs from ZX Interface 1, creating a "header" and "bytes" file.

blocks2tzx
* this is a simple hack to make a TZX usable in a modern emulator from files created by getser.

mdrinfo
* rough program to "map" Microdrive cartridge file, also showing cart name and a simple catalog

mdr2bas
* rough program to extract BASIC programs from Microdrive cartridge file, saving as ".bas" PLUS3DOS format (e.g. can load onto ZX Spectrum Next).
