; arg parsing

parse_arguments:
	; check for no tail
	ld	a,h
	or	l
	jr	z,show_usage

	; have tail
	ld	(command_tail),hl

parse_firstarg:
	; get an argument, check if option and process
	call	get_sizedarg
	jr	nc,show_usage
	call	check_options
	jr	z,parse_firstarg

	; start processing
	jr	main_start

show_usage:
	ld	hl,msg_help
	call	printmsg
	and	a
	jr	error_handler

