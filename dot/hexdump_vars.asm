; vars

msg_help:
	defm	"HEXDUMP v1.0 by Brian Handscomb",$0d
	defm	"USAGE:",$0d
	defm	" .HEXDUMP [OPT] FILE",$0d
	defm	"OPTIONS:",$0d
	defm	" -w   wide mode",$0d
	defm	$ff

msg_badnextzxos:
	defm	"Requires NextZXO",'S'+$80	; BASIC style error

msg_weird:
	defm	"Weird read (>8)",$ff

temparg:
	defs	256

saved_sp:
	defw	0

saved_turbo:
	defb	0

command_tail:
	defw	0

mode_wide:
	defb	0

header_buff:
	defs	8	; don't need it, but just in case

fhandle:
	defb	0
offsethi:
	defb	0
offset:
	defw	0
numbytes:
	defb	0
pad:
	defb	0
byte_buffer:
	defs	8

