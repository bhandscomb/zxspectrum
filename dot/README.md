# DOT commands

.hexdump [-w] "file"

Simple hexdump tool. Pretty much works though no way to stop it once it is going unless you are in the classic 32 column mode and you BREAK at the Scroll? prompt.

Code isn't too good/efficient but not going for code quality, going for something I'm going to find useful.

Coded using an enhanced (z00m128) version of SjASMPlus that supports Z80N instructions and the CSpect emulator for testing.

It is recommended to place the filename to hexdump in (double) quotes, but this if the filename/path has no spaces you should be able to get away without quotes.

Optionally you can provide the "-w" switch for a wide output intended for use in hires screen modes, most usefully using the NextZXOS command-line interface. The hex dump is in a slightly clearer fashion and the file position indicator supports true position reporting beyond 64K (uses a "24-bit counter").

