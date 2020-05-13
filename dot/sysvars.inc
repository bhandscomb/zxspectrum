; system variables

SWAP		equ	$5B00	; paging subroutine
STOO		equ	$5B10	; paging subroutine
YOUNGER		equ	$5B21	; paging subroutine
REGNUOY		equ	$5B2A	; paging subroutine
ONERR		equ	$5B3A	; paging subroutine

OLDHL		equ	$5B52	; temporary register store while switching ROMs
OLDBC		equ	$5B54	; temporary register store while switching ROMs
OLDAF		equ	$5B56	; temporary register store while switching ROMs

TARGET		equ	$5B58	; subroutine address in ROM 3
RETADDR		equ	$5B5A	; return address in ROM 1

BANKM		equ	$5B5C	; last output to I/O port 7FFDh

RAMRST		equ	$5B5D	; RST 8 - ROM 1 report old errors to ROM 3
RAMERR		equ	$5B5D	; error number passed from ROM 1 to ROM 3

INKL		equ	$5B5F	; INK for LoRes
INK2		equ	$5B60	; INK for Layer 2
ATTRULA		equ	$5B61	; Attributes for standard mode
ATTRHR		equ	$5B62	; Attributes for HiRes (paper bits 3 to 5)
ATTRHC		equ	$5B63	; Attributes for HiColour
INKMASK		equ	$5B54	; Softcopy of EnhancedULA InkMask (or 0)

LSBANK		equ	$5B65	; Temp bank number in LOAD/SAVE etc
FLAGS3		equ	$5B66	; Various flags
BANK678		equ	$5B67	; last output to I/O port 1FFDh
FLAGN		equ	$5B68	; flags for the NextZXOS system
MAXBNK		equ	$5B69	; maximum available RAM bank
OLDSP		equ	$5B6A	; old SP when TSTACK is in use
SYNRET		equ	$5B6C	; return address for ONERR
LASTV		equ	$5B6E	; last value printed by calculator

TILEBNKL	equ	$5B73	; tiles bank for LoRes
TILEML		equ	$5B74	; tilemap bank for LoRes
TILEBNK2	equ	$5B75	; tiles bank for Layer 2
TILEM2		equ	$5B76	; tilemap bank for Layer 2

NXTBNK		equ	$5B77	; bank containing NXTLIN
DATABNK		equ	$5B78	; bank containing DATADD

LODDRV		equ	$5B79	; LOAD, etc, drive (T from Tape, else A/B/M)
SAVDRV		equ	$5B7A	; SAVE drive (T drom Tape, else A/B/M)

L2SOFT		equ	$5B7B	; softcopy of Layer 2 port

TILEWL		equ	$5B7C	; width of LoRes tilemap
WILEW2		equ	$5B7E	; width of Layer 2 tilemap
TILEOFFL	equ	$5B80	; offset in bank for LoRes tilemap
TILEOFF2	equ	$5B82	; offset in bank for Layer 2 tilemap

COORDSX		equ	$5B84	; x coord of last point plotted (Layer 1/2)
COORDSY		equ	$5B86	; y coord of last point plotted (Layer 1/2)

PAPERL		equ	$5B88	; PAPER colour for LoRes mode
PAPER2		equ	$5B89	; PAPER colour for Layer 2 mode

TMPVARS		equ	$5B8A	; base of temporary sys vars (shared TSTACK)
TSTACK		equ	$5BFF	; temp stack grows down from here

; now the classics

KSTATE		equ	$5C00
LASTK		equ	$5C08
REPDEL		equ	$5C09
REPPER		equ	$5C0A
DEFADD		equ	$5C0B
K_DATA		equ	$5C0D
TVDATA		equ	$5C0E
STRMS		equ	$5C10
CHARS		equ	$5C36	; very forward thinking for 1982, IMHO
RASP		equ	$5C38
PIP		equ	$5C39
ERRNR		equ	$5C3A
FLAGS		equ	$5C3B
TVFLAG		equ	$5C3C
ERRSP		equ	$5C3D
LISTSP		equ	$5C3F
MODE		equ	$5C41
NEWPPC		equ	$5C42
NSPPC		equ	$5C44
PPC		equ	$5C45
SUBPPC		equ	$5C47
BORDCR		equ	$5C48
E_PPC		equ	$5C49
VARS		equ	$5C4B
DEST		equ	$5C4D
CHANS		equ	$5C4F
CURCHL		equ	$5C51
PROG		equ	$5C53
NXTLIN		equ	$5C55	; typo in Next manual - says $5C57 in hex
DATADD		equ	$5C57
E_LINE		equ	$5C59
K_CUR		equ	$5C5B
CH_ADD		equ	$5C5D
X_PTR		equ	$5C5F
WORKSP		equ	$5C61
STKBOT		equ	$5C63
STKEND		equ	$5C65
BREG		equ	$5C67
MEM		equ	$5C68
FLAGS2		equ	$5C6A
DF_SZ		equ	$5C6B
S_TOP		equ	$5C6C
OLDPPC		equ	$5C6E
OSPPC		equ	$5C70
FLAGX		equ	$5C71
STRLEN		equ	$5C72
T_ADDR		equ	$5C74
SEED		equ	$5C76
FRAMES		equ	$5C78
UDG		equ	$5C7B
COORDS		equ	$5C7D	; X first, Y second

GMODE		equ	$5C7F	; graphical layer/mode flags
				; this was P_POSN in classic

PRCC		equ	$5C80	; this was two bytes in classic and the
				; Next manual says is still two bytes but
				; the second byte seems to be re-used

STIMEOUT	equ	$5C81	; screensaver control

ECHO_E		equ	$5C82
DF_CC		equ	$5C84
DF_CCL		equ	$5C86
S_POSN		equ	$5C88	; column first, line second
SPOSNL		equ	$5C8A	; column first, line second
SCR_CT		equ	$5C8C
ATTR_P		equ	$5C8D
MASK_P		equ	$5C8E
ATTR_T		equ	$5C8F
MASK_T		equ	$5C90
P_FLAG		equ	$5C91
MEMBOT		equ	$5C92
NMIADD		equ	$5CB0	; says unused in Next
RAMTOP		equ	$5CB2
P_RAMT		equ	$5CB4

S
