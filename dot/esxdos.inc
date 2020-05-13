; api - esxDOS compatible API

; Low-level calls

disk_filemap		equ	$85
disk_strmstart		equ	$86
disk_strmend		equ	$87

; Miscellaneous calls

m_dosversion		equ	$88
m_getsetdrv		equ	$89
m_tapein		equ	$8b
m_tapeout		equ	$8c
m_gethandle		equ	$8d
m_getdate		equ	$8e
m_execcmd		equ	$8f
m_setcaps		equ	$91
m_drvapi		equ	$92
m_geterr		equ	$93
m_p3dos			equ	$94
m_errh			equ	$95

; File calls

f_open			equ	$9a
f_close			equ	$9b
f_sync			equ	$9c
f_read			equ	$9d
f_write			equ	$9e
f_seek			equ	$9f
f_fgetpos		equ	$a0
f_fstat			equ	$a1
f_ftruncate		equ	$a2
f_opendir		equ	$a3
f_readdir		equ	$a4
f_telldir		equ	$a5
f_seekdir		equ	$a6
f_rewinddir		equ	$a7
f_getcwd		equ	$a8
f_chdir			equ	$a9
f_mkdir			equ	$aa
f_rmdir			equ	$ab
f_stat			equ	$ac
f_unlink		equ	$ad
f_truncate		equ	$ae
f_chmod			equ	$af
f_rename		equ	$b0
f_getfree		equ	$b1

; error codes

esx_ok			equ	0
esx_eok			equ	1
esx_nonsense		equ	2
esx_estend		equ	3
esx_ewrtype		equ	4
esx_enoent		equ	5
esx_eio			equ	6
esx_einval		equ	7
esx_eacces		equ	8
esx_enospc		equ	9
esx_enxio		equ	10
esx_enodrv		equ	11
esx_enfile		equ	12
esx_ebadf		equ	13
esx_enodev		equ	14
esx_eoverflow		equ	15
esx_eisdir		equ	16
esx_enotdir		equ	17
esx_eexist		equ	18
esx_epath		equ	19
esx_esys		equ	20
esx_enametoolong	equ	21
esx_enocmd		equ	22
esx_einuse		equ	23
esx_erdonly		equ	24
esx_everify		equ	25
esx_eloadingko		equ	26
esx_edirinuse		equ	27
esx_emapramactive	equ	28
esx_edrivebusy		equ	29
esx_efsunknown		equ	30
esx_edevicebusy		equ	31

; m_tapein
in_open			equ	0
in_close		equ	1
in_info			equ	2
in_setpos		equ	3
in_getpos		equ	4
in_pause		equ	5
in_flags		equ	6

; m_tapeout
out_open		equ	0
out_close		equ	1
out_info		equ	2
out_trunc		equ	3

; f_open
esx_mode_read		equ	$01
esx_mode_write		equ	$02
esx_mode_use_header	equ	$40
esx_mode_open_exist	equ	$00
esx_mode_open_creat	equ	$08
esx_mode_creat_noexist	equ	$04
esx_mode_creat_trunc	equ	$0c

; f_seek
esx_seek_set		equ	$00
esx_seek_fwd		equ	$01
esx_seek_bwd		equ	$02

; f_opendir
esx_mode_use_lfn	equ	$10
esx_mode_use_wildcards	equ	$20
;esx_mode_use_header	equ	$40	; see f_open

; f_chmod
A_WRITE			equ	%00000001
A_READ			equ	%10000000
A_RDWR			equ	%10000001
A_HIDDEN		equ	%00000010
A_SYSTEM		equ	%00000100
A_ARCH			equ	%00100000

