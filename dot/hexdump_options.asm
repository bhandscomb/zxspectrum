; options

check_options:
	ld	a,(temparg)
	cp	'-'
	ret	nz
	ld	a,(temparg+1)
	cp	'w'
	jr	z,setwide
	cp	'W'
	jr	z,setwide
	ret
setwide:
	ld	a,1
	ld	(mode_wide),a
	xor	a
	ret

