; error

bad_nextzxos:
	ld	hl,msg_badnextzxos
	xor	a
	scf

error_handler:
	ld	sp,(saved_sp)
restore_all:
	push	af
	ld	a,(saved_turbo)
	nextreg	nxr_turbo,a
	pop	af
	ret

stderr_handler:
	call	cleanup		; try and be tidy
	call	restore_all
	ld	h,a
	ld	l,$cf		; RST 8
	ld	(RAMRST),hl
	ld	hl,0
	callesx	m_errh
	call48k	RAMRST

