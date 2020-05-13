; printmsg

printmsg:
	ld	a,(hl)
	inc	hl
	inc	a
	ret	z	; $ff to exit
	dec	a
	print_char
	jr	printmsg

