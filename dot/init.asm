; init

	; save SP for error handler
	ld	(saved_sp),sp

	; save address of args
	push	hl

	; get turbo setting
	ld	bc,next_reg_select
	ld	a,nxr_turbo
	out	(c),a
	inc	b
	in	a,(c)
	ld	(saved_turbo),a

	; set to max turbo
	ld	a,turbo_max
	out	(c),a

	; get esxDOS version
	callesx	m_dosversion
	jr	c,bad_nextzxos

	; check NextZXOS signature
	ld	hl,('N'<<8)+'X'
	sbc	hl,bc
	jr	nz,bad_nextzxos

	; check version >= 1.99
	ld	hl,$0199
	ex	de,hl
	sbc	hl,de
	jr	c,bad_nextzxos

	; install error handler to reset turbo
	ld	hl,stderr_handler
	callesx	m_errh

	; restore address of args
	pop	hl

