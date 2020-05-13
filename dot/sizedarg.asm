; get_sizedarg

get_sizedarg:
	ld	hl,(command_tail)
	ld	de,temparg
	ld	bc,0
get_sizedarg_loop:
	ld	a,(hl)
	inc	hl
	and	a			; null terminates
	ret	z
	cp	$0d			; CR terminates
	ret	z
	cp	':'			; : terminates
	ret	z
	cp	' '			; skip spaces
	jr	z,get_sizedarg_loop
	cp	'"'			; quoted arg
	jr	z,get_sizedarg_quoted
get_sizedarg_unquoted:
	ld	(de),a
	inc	de
	inc	c
	jr	z,get_sizedarg_badsize
	ld	a,(hl)
	and	a
	jr	z,get_sizedarg_complete
	cp	$0d
	jr	z,get_sizedarg_complete
	cp	':'
	jr	z,get_sizedarg_complete
	cp	'"'
	jr	z,get_sizedarg_complete
	inc	hl
	cp	' '
	jr	nz,get_sizedarg_unquoted
get_sizedarg_complete:
	xor	a
	ld	(de),a
	ld	(command_tail),hl
	scf
	ret
get_sizedarg_quoted:
	ld	a,(hl)
	and	a
	jr	z,get_sizedarg_complete
	cp	$0d
	jr	z,get_sizedarg_complete
	inc	hl
	cp	'"'
	jr	z,get_sizedarg_checkendquote
	ld	(de),a
	inc	de
	inc	c
	jr	z,get_sizedarg_badsize
	jr	get_sizedarg_quoted
get_sizedarg_badsize
	pop	af
	jp	show_usage
get_sizedarg_checkendquote
	inc	c
	dec	c
	jr	z,get_sizedarg_complete
	dec	de
	ld	a,(de)
	inc	de
	cp	'\'
	jr	nz,get_sizedarg_complete
	dec	de
	ld	a,'"'
	ld	(de),a
	inc	de
	jr	get_sizedarg_quoted

