; include files (no code)

esxdos.inc		API for esxDOS compatible calls (complete)
hardware.inc		ZX Spectrum Next hardware (extracts)
macros.inc		Useful macros
sysvars.inc		System variables (complete)

; general assembly

args.asm		Code for arg parse, must be after init.asm
error.asm		Error routines
init.asm		Init code, must be ORG $2000
printmsg.asm		Message printer
sizedarg.asm		Code to pull a single arg from command line

; program assembly
hexdump.asm		Main code
hexdump_options.asm	Parse options
hexdump_vars.asm	Variables
