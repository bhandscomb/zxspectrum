; macros

	macro	call48k	address
	rst	$18
	defw	address
	endm

	macro	callesx	hook
	rst	$8
	defb	hook
	endm

	macro	print_char
	rst	$10
	endm

