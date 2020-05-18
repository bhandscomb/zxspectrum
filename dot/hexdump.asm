; hexdump.asm
;
; .HEXDUMP [OPT] FILE
; OPTIONS:
;  -w, --wide
;      wide mode

; 00000000001111111111222222222233|33333333444444444455555555556666
; 01234567890123456789012345678901|23456789012345678901234567890123
;
; normal format (64K wrapping offset
; @0000 0000000000000000 XXXXXXXX
; wide format
; @000000  00 00 00 00  00 00 00 00  XXXXXXXX
;
; SJASMPlus

	; save having to do this on command line
	OPT	push reset --zxnext
	OUTPUT	hexdump

	include	"hardware.inc"
	include	"macros.inc"
	include	"esxdos.inc"
	include	"sysvars.inc"

; ***
; init
; ***

	org	$2000

hexdump_init:

	include	"init.asm"		; common init code
	; fall through to...
	include	"args.asm"		; parse arguments (mostly common)
	include	"error.asm"		; error routines

main_start:
	ld	a,'*'
	ld	hl,temparg	; temparg but we fix it for testing
	ld	de,header_buff
	ld	b,esx_mode_read+esx_mode_open_exist
	callesx	f_open
	jr	c,error_handler
	ld	(fhandle),a

loop:
	; first let's test for break (CAPS+SPACE in old money)
	ld	bc,keyb_CAPS_V
	in	a,(c)
	and	$1f
	cp	$1e
	jr	nz,not_break
	ld	bc,keyb_SPACE_B
	in	a,(c)
	and	$1f
	cp	$1e
	jr	nz,not_break
	ld	a,$c		; break error
	jp	stderr_handler
not_break:
	ld	hl,byte_buffer
	ld	bc,8
	ld	a,(fhandle)
	callesx	f_read
	ld	a,b		; should be zero
	or	a
	jr	nz,weird
	or	c		; if this zero too must be done
	jr	z,zero_bytes
	cp	9		; short, full, or weird?
	jr	nc,weird
	;jr	z,full8		; actually don't do anything if full read
	; short read, calc pad
	ld	a,8
	sub	c
	ld	(pad),a
	ld	a,c
	ld	(numbytes),a
	jr	printrow

weird:
	ld	hl,msg_weird
	call	printmsg
	call	cleanup
	jr	finished

zero_bytes:
	call	cleanup

finished:
	and	a
	jp	error_handler

cleanup:
	push	af		; needed in case called from error handler
	ld	a,(fhandle)
	and	a
	jr	z,notopen
	xor	a
	ld	(fhandle),a
	callesx	f_close
notopen:
	pop	af
	ret

printrow:
	; (1) offset, starting @
	ld	a,'@'
	print_char
	ld	a,(mode_wide)
	or	a
	jr	z,pr_off_low
	ld	a,(offsethi)
	call	pr2h
pr_off_low:
	ld	a,(offset+1)
	call	pr2h
	ld	a,(offset)
	call	pr2h
	; (2) space(s) before hex part
	ld	a,' '
	print_char
	ld	a,(mode_wide)
	or	a
	jr	z,pr_hex
	ld	a,' '
	print_char
pr_hex:
	ld	hl,byte_buffer
	ld	e,l		; where to add extra space
	inc	e
	inc	e
	inc	e
	inc	e
	ld	a,(numbytes)
	ld	b,a
pr_hex_loop:
	ld	a,(hl)
	inc	hl
	call	pr2h
	ld	a,(mode_wide)
	or	a
	jr	z,no_extra_pad
	ld	a,' '
	print_char
	ld	a,l
	cp	e
	jr	nz,no_extra_pad
	ld	a,' '
	print_char
no_extra_pad:
	djnz	pr_hex_loop
	; (3) padding before ascii
	ld	a,(pad)		; calc padding
	ld	c,a			; c=pad
	ld	a,1			; a=1
	add	c			; a=1+pad+pad
	add	c
	ld	b,a			; save to use, b=1+pad+pad
	ld	a,(mode_wide)		; mode check
	or	a
	jr	z,print_pad
	ld	a,b			; a=1+pad+pad
	add	c			; a=1+pad+pad+pad
	ld	b,a			; save to use, b=1+pad+pad+pad
	ld	a,(pad)			; extra space if lots of pad?
	cp	4
	jr	c,print_pad
	inc	b			; update, b=2+pad+pad+pad
print_pad:
	ld	a,' '
	print_char
	djnz print_pad
	; (4) ascii
	ld	hl,byte_buffer
	ld	a,(numbytes)
	ld	b,a
ascii_loop:
	ld	a,(hl)
	inc	hl
	cp	32
	jr	c,badchar
notlow:
	cp	127
	jr	c,nothigh
badchar:
	ld	a,'.'
nothigh:
	print_char
	djnz	ascii_loop
	; (5) end of line and repeat (except to test, no repeat)
	ld	a,13
	print_char
	ld	a,(numbytes)
	ld	e,a
	ld	d,0
	ld	hl,(offset)
	add	hl,de
	ld	(offset),hl
	jp	nc,loop
	ld	hl,offsethi
	inc	(hl)
	jp	loop

pr2h:
	push	af		; save hex byte
	rrca			; move top nibble down
	rrca
	rrca
	rrca
	and	$0f
	cp	10		; need to adjust for A-F or okay?
	jr	c,pr2h_x1
	add	a,7		; adjust
pr2h_x1:
	add	a,$30		; offset to char code
	print_char		; print
	pop	af		; restore hex byte, low nibble as per high
	and	$0f
	cp	10
	jr	c,pr2h_x2
	add	a,7
pr2h_x2:
	add	a,$30
	print_char
	ret

	include	"sizedarg.asm"		; util: get argument from command line
	include	"hexdump_options.asm"	; our options (wide mode)
	include	"printmsg.asm"		; util: message print (end $ff)
	include	"hexdump_vars.asm"	; variable storage


