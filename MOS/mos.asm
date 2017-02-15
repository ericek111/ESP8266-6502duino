.org $FC00

language	= $8000
title   	= $8009

stack		= $100
serbuf		= $300	; Serial input buffer $300-$3FF
scratchbuf	= $280	; $280-$2FF

osfileblock	= $D1	; $D1-$DD
oscliscratch1	= $DE	; $DE-$DF
oscliscratch2	= $E0	; $E0-$E1
oscliaddr	= $E2	; $E2-$E3
timer		= $E4	; $E4-$E8
rdchscratch	= $E9	; 1 byte scratch space.
wordscratch	= $EA	; 1 byte scratch space.
irqscratch	= $EB	; 1 byte scratch space.
printaddr	= $EC	; $EC-$ED
oswordA		= $EE
oswordPTR	= $EF	; $EF-$F0
pserin		= $F1	; Serial input pointer (writing to buffer).
pserout		= $F2	; Serial output pointer (reading from buffer).
oswordscratch	= $F3	; $F3-FC

; DO NOT CHANGE - these are used by BBC BASIC.
;
brkdata		= $FD	; $FD-$FE
escapeflag	= $FF	; If escape pressed, top bit gets set.
brkv            = $0202	; BASIC overrides the BRK vector for error handling.
wrchv           = $020E ; BASIC jumps via WRCHV sometimes.

.include "hardware.asm"
.include "coprocessor.asm"
.include "vectors.asm"

; **************************************************
; Reset handler. Sets up BRK and WRCH vectors,
; outputs our banner and the language title from the
; BASIC ROM, and calls the language entry point.

_reset_handler:	
		; Setup the BRK and WRCH vectors
		; with interrupts disabled.
		SEI
		LDA #<_osbrk
		STA brkv
		LDA #>_osbrk
		STA brkv+1

		LDA #<_oswrch
		STA wrchv
		LDA #>_oswrch
		STA wrchv+1
		CLI

		JSR OSNEWL

		LDX #<_banner
		LDY #>_banner
		JSR OSPRINT
		JSR OSNEWL
		JSR OSNEWL

		LDX #<title
		LDY #>title
		JSR OSPRINT
		JSR OSNEWL
		JSR OSNEWL

		; Language entry point
		LDA #1
		JMP language

_banner:	.byte "Alun's MOS shim", 13

; **************************************************
; Custom vector - not present on the BBC. This sets or
; reads the emulator instruction counter. If A=0 then
; zeros the counter, otherwise stores it to 4 bytes at 
; $YYXX
_osicounter:
		.byte COP, COP_ICOUNTER
		RTS

; **************************************************
; Custom vector - not present on the BBC. This resets
; the emulator using a coprocessor call.
_osreboot:
		.byte COP, COP_REBOOT
		RTS	; Should never be reached!


; **************************************************
; Custom vector - not present on the BBC. This outputs
; a NUL terminated string.
_osprint:	PHA
		STX printaddr
		STY printaddr+1
		LDY #0
_ploop:		LDA (printaddr), Y
		BEQ _pdone
		CMP #13
		BEQ _pdone
		JSR OSWRCH
		INY
		JMP _ploop
_pdone:		LDY printaddr+1
		PLA
		RTS

; **************************************************
; Custom vector - not present on the BBC. This outputs
; a HEX byte.
_osphex:	PHP
		PHA
		PHA
		LSR
		LSR
		LSR
		LSR
		JSR _nybble
		PLA
		AND #15
		JSR _nybble
		PLA
		PLP
		RTS

; Output a hex nybble from A. Destroys A.
_nybble:	CMP #10
		; Under 10, it's a digit. Otherwise we
		; need to add 7 to get to 'A'.
		BCC _under10
		ADC #6 				; (plus the carry)
_under10:	ADC #'0'			; Carry is clear here
		JSR OSWRCH
		RTS

; **************************************************
; Destination of the NMI vector. Unused in our emulation.
; Also the initial destination of (brkv).

_nmi_handler:	
_osbrk:		RTI


; **************************************************
; Destination of the IRQ vector. This decides whether the
; IRQ is a result of BRK or hardware. In the former case,
; it fixes the stack can jumps through (brkv). In the latter
; it handles the IRQs from our emulation.
;
; More or less verbatim from
; http://mdfs.net/Docs/Comp/BBC/OS1-20/DC1C

_irq_handler:
		STA irqscratch
		PLA
		PHA
		AND #$10
		BEQ _real_irq

		; BRK handler. Need to set up some stuff
		; for BASIC.
		TXA
		PHA
		TSX
		LDA stack+3, X
		CLD
		SEC
		SBC #1
		STA brkdata
		LDA stack+4, X
		SBC #0
		STA brkdata+1
		PLA
		TAX
		LDA irqscratch
		CLI
		JMP (brkv)

; **************************************************
; The real IRQ handlers

_real_irq:	
		; Get IFLAG and check for serial IRQ.
		LDA #0
		.byte COP, COP_IFLAG
		AND #IFLAG_SERIAL
		BNE _serial_irq

		; Ditto timer IRQ.
		LDA #0
		.byte COP, COP_IFLAG
		AND #IFLAG_TIMER
		BNE _timer_irq

		LDA irqscratch
		RTI

_serial_irq:
		TYA
		PHA
		LDY pserin
		.byte COP, COP_GETCHAR
		STA serbuf, Y
		INC pserin
		CMP #27		; Escape
		BNE _notescape
		LDA #$80
		ORA escapeflag
		STA escapeflag
_notescape:		
		PLA
		TAY
		LDA irqscratch
		RTI

_timer_irq:
		CLC
		LDA timer
		ADC #10		; ESP timer is 10Hz, but we want to look like 100Hz for BBC BASIC
		STA timer
		BCC _tirq_done
		INC timer+1
		BNE _tirq_done
		INC timer+2
		BNE _tirq_done
		INC timer+3
		BNE _tirq_done
		INC timer+4
_tirq_done:	
		; Clear IRQ flag.
		LDA #IFLAG_TIMER
		.byte COP, COP_IFLAG

		LDA irqscratch
		RTI
		
; **************************************************
; Read character from input stream into A.
_osrdch:	
		STY rdchscratch
		; Busy loop for input to arrive (via interrupt).
_rdchwait:	LDY pserout
		CPY pserin
		BEQ _rdchwait
		SEI
		LDA escapeflag
		ROL A				; Set C if escape has been pressed.
		LDA serbuf, Y
		INC pserout
		LDY rdchscratch
		CLI
		CLD
		RTS

; **************************************************
; Output a character from A.
_oswrch:
		.byte COP, COP_SENDCHAR
		CLD
		RTS

; **************************************************
; OSBYTE Routines. Need to emulate a few of them for BBC BASIC.
_osbyte:
		CLD
		CMP #$7E
		BEQ _osbyte126
		CMP #$81
		BEQ _osbyte129
		CMP #$82
		BEQ _osbyte130
		CMP #$83
		BEQ _osbyte131
		CMP #$84
		BEQ _osbyte132
		CMP #$85
		BEQ _osbyte133
		RTS

_osbyte126:	; Clear escape flag
		PHA
		SEI
		LDX #0
		LDA escapeflag
		BPL _noescpending
		AND #$7F
		STA escapeflag
		INX
_noescpending:
		CLI
		PLA
		RTS

_osbyte129:	; Read key with time limit
		; Cop out for now.
		LDY #$FF
		SEC
		RTS

_osbyte130:	; Read machine high order address
		LDX #$FF
		LDY #$FF
		RTS

_osbyte131:	; Read top of OS RAM address ($0800)
		LDY #$08
		LDX #0
		RTS

_osbyte132:	; Read bottom of display RAM address ($4000)
_osbyte133:	; Read bottom of display RAM for specified mode ($4000)

		; For our emulation, this is actually the top of
		; RAM. And we've a coprocessor operation for that.
		.byte COP, COP_RAMTOP

		TAY
		LDX #0
		LDA #133
		RTS

; **************************************************
; OSWORD Routines. Need to emulate a few of them for BBC BASIC.

_osword:	; Miscellanous operations.
		CLD
		STA oswordA
		STX oswordPTR
		STY oswordPTR+1
		CMP #0
		BEQ _inputline
		CMP #1
		BEQ _readclock
		CMP #2
		BEQ _writeclock
		RTS

; Read TIME.
_readclock:	LDY #4
		SEI
_rcloop:	LDA timer, Y
		STA (oswordPTR), Y
		DEY
		BPL _rcloop
		CLI
		LDA oswordA
		LDY oswordPTR+1
		RTS

; Write TIME.
_writeclock:	LDY #4
		SEI
_wcloop:	LDA (oswordPTR), Y
		STA timer, Y
		DEY
		BPL _wcloop
		CLI
		LDA oswordA
		LDY oswordPTR+1
		RTS

; Get a line of text.
_inputline:	

		; Get the string buffer address.
		LDY #0
		LDA (oswordPTR), Y
		STA oswordscratch
		INY
		LDA (oswordPTR), Y
		STA oswordscratch+1

		; Maximum line length
		INY
		LDA (oswordPTR), Y
		STA oswordscratch+2
		
		; Minimum acceptable character
		INY
		LDA (oswordPTR), Y
		STA oswordscratch+3

		; Maximum acceptable character
		INY
		LDA (oswordPTR), Y
		STA oswordscratch+4

		LDY #0
_inputloop:	JSR OSRDCH
		BCS _inputescape
		CMP #10			; LF
		BEQ _inputdone
		CMP #13			; CR
		BEQ _inputdone
		CMP #21			; ^U
		BEQ _inputwipe
		CMP #8
		BEQ _inputdel
		CMP #127		; DEL
		BNE _inputnotdel
_inputdel:	CPY #0
		BEQ _inputloop
		DEY
		JSR _inputbackspace
		JMP _inputloop
_inputnotdel:	
		CMP oswordscratch+3
		BCC _inputloop
		CMP oswordscratch+2
		BCS _inputloop
		CPY oswordscratch+4
		BEQ _inputloop
		STA (oswordscratch), Y
		JSR OSWRCH
		INY
		BNE _inputloop
_inputdone:	
		LDA #13
		STA (oswordscratch), Y
		JSR OSNEWL
		CLC
_inputescape:	LDA oswordA
		RTS

_inputwipe:	
		CPY #0
		BEQ _inputloop
		JSR _inputbackspace
		DEY
		JMP _inputwipe

_inputbackspace:
		LDA #8
		JSR OSWRCH
		LDA #32
		JSR OSWRCH
		LDA #8
		JMP OSWRCH

; **************************************************
; OSCLI - "*" commands. We need a few to do with file
; handling. The parser on this is very minimal, so 
; don't expect miracles!

; Here's the table of commands. We have CR-terminated
; strings, followed by addresses to jump to on match.
_osclitab:
		.byte "cat", 13
		.addr _star_cat

		.byte ".", 13
		.addr _star_cat

		.byte "delete", 13
		.addr _star_delete

		.byte "reboot", 13
		.addr OSREBOOT

		.byte "help", 13
		.addr _star_help

		.byte "free", 13
		.addr _star_free

		; Terminate table with blank command.
		.byte 13

; The actual entry point for OSCLI.
_oscli:
		STX oscliaddr
		STY oscliaddr+1

		LDA #<_osclitab
		STA oscliscratch2
		LDA #>_osclitab
		STA oscliscratch2+1

		; Strip off leading stars.
		LDY #0
_oscliskipstar:	LDA (oscliaddr), Y
		CMP #'*'
		BNE _osclicmdloop
		INC oscliaddr
		BNE _oscliskipstar
		INC oscliaddr+1
		BNE _oscliskipstar

_osclicmdloop:	
		LDA oscliaddr
		STA oscliscratch1
		LDA oscliaddr+1
		STA oscliscratch1+1

		LDX oscliscratch2
		LDY oscliscratch2+1

		LDY #0

		; Compare command line against table entry.
_osclicompare:	LDA (oscliscratch1), Y

		; Use space or CR to terminate.
		CMP #' '
		BNE _osclinotspace
		LDA #13
_osclinotspace:	
		CMP #'A'
		BCC _osclinotcaps
		CMP #'Z'
		BCS _osclinotcaps
		ORA #32

_osclinotcaps:	CMP (oscliscratch2), Y
		BNE _osclinotmatched

		; They match. Now was it CR?
		CMP #13

		; Yes, found command.
		BEQ _osclimatched

		; Not yet found, but not wrong either.
		INY
		BNE _osclicompare

_osclinotmatched:
		; Scan forward to next command in table.
		LDA (oscliscratch2), Y
		CMP #13
		BEQ _osclinextcmd
		INY
		BNE _osclinotmatched
_osclinextcmd:
		; Skip the command pointer.
		INY
		INY
		INY

		; Now add Y to the table pointer ready for the next command.
		TYA
		CLC
		ADC oscliscratch2
		STA oscliscratch2
		LDA oscliscratch2+1
		ADC #0
		STA oscliscratch2+1

		; Check whether we're at the end of the table.
		LDY #0
		LDA (oscliscratch2), Y
		CMP #13
		BNE _osclicmdloop

		BRK
		.byte 255
		.byte "Bad command", 0

_osclimatched:
		INY
		LDA (oscliscratch2), Y
		STA oscliscratch1
		INY
		LDA (oscliscratch2), Y
		STA oscliscratch1+1
		DEY
		TYA
		CLC
		ADC oscliaddr
		STA oscliaddr
		LDA #0
		ADC oscliaddr+1
		STA oscliaddr+1
		JMP (oscliscratch1)

; Catalogue - simply list names of all files.
_star_cat:
		LDA #0
		LDY #12
_catclr:	STA osfileblock, Y
		DEY
		BPL _catclr
_catloop:	LDA #1
		STA osfileblock+5
		LDA #<scratchbuf
		STA osfileblock+1
		LDA #>scratchbuf
		STA osfileblock+2
		LDA #8
		LDX #<osfileblock
		LDY #>osfileblock
		JSR OSGBPB
		BCS _rts1
		LDY #0
_catprint:	LDA scratchbuf+1, Y
		JSR OSWRCH
		INY
		CPY scratchbuf
		BNE _catprint
		JSR OSNEWL
		JMP _catloop
_rts1:		RTS

; Delete named file.
_star_delete:
		LDA #6
		LDX #<oscliaddr
		LDY #>oscliaddr
		JMP OSFILE

; List available commands.
_star_help:
		LDX #<_helpbanner
		LDY #>_helpbanner
		JSR OSPRINT
		JSR OSNEWL

		LDA #<_osclitab
		STA oscliscratch2
		LDA #>_osclitab
		STA oscliscratch2+1
_helploop:	LDY #0

_helpprint:	LDA (oscliscratch2), Y
		JSR OSASCI
		INY
		CMP #13
		BNE _helpprint

		; Empty command string is end of list.
		CPY #1
		BEQ _rts1

		; Skip the jump address and add to our pointer.
		; So our pointer should now be pointing at the start
		; of the next command.
		INY
		INY
		TYA
		CLC
		ADC oscliscratch2
		STA oscliscratch2
		LDA #0
		ADC oscliscratch2+1
		STA oscliscratch2+1
		JMP _helploop

_helpbanner:	.byte "Commands:", 13

_star_free:	
		LDA #4
		LDY #0
		LDX #oscliscratch1
		JSR OSARGS
		JSR _phex32
		LDY #0
		LDA #','
		JSR OSWRCH
		LDA #5
		JSR OSARGS
		JSR _phex32
		JMP OSNEWL

_phex32:	LDY#3
_ph32loop:	LDA oscliscratch1, Y
		JSR OSPHEX
		DEY
		BPL _ph32loop
		RTS

; **************************************************
; OSFIND - Open/close files - Coprocessor call.
_osfind:
		.byte COP, COP_OSFIND
		CLD
		RTS
		
; **************************************************
; OSFILE - Whole file operations - Coprocessor call.
_osfile:	.byte COP, COP_OSFILE
		BCC _osfileOK
		BRK
		.byte 255
		.byte "I/O failed", 0
_osfileOK:	CLD
		RTS

; **************************************************
; OSBGET - Byte input - Coprocessor call.
_osbget:	.byte COP, COP_OSBGET
		CLD
		RTS

; **************************************************
; OSBPUT - Byte output - Coprocessor call.
_osbput:	.byte COP, COP_OSBPUT
		CLD
		RTS

; **************************************************
; OSGBPB - Read or write multiple bytes of data.
_osgbpb:	.byte COP, COP_OSGBPB
		CLD
		RTS

; **************************************************
; OSARGS - Read information on open files
_osargs:	.byte COP, COP_OSARGS
		CLD
		RTS

; **************************************************
; Not implemented routines. Just generate an error.

_osnotimp:	BRK
		.byte 255
		.byte "Not implemented", 0

; **************************************************
; MOS entry points. Some are unimplemented. Unlike on the BBC,
; most are not vectored.

.res		OSICOUNTER-*		; OSICOUNER *NOT BBC* - get or set icounter
		JMP _osicounter
.res		OSREBOOT-*		; OSREBOOT *NOT BBC* - reboot emulator.
		JMP _osreboot
.res		OSPRINT-*		; OSPRINT *NOT BBC* - print NUL terminated string from location in $YYXX
		JMP _osprint
.res		OSPHEX-*		; OSPHEX  *NOT BBC* - print 2 byte hex from value in A
		JMP _osphex
.res		OSRDSC-*		; OSRDSC - Read a byte from screen or sideways ROM/RAM
		JMP _osnotimp
.res		OSVDU-*			; OSVDU  - Raw VDU entry point
		JMP _osnotimp
.res		OSEVEN-*		; OSEVEN - Generate an event
		JMP _osnotimp
.res		GSINIT-*		; GSINIT - Initialise string reading
		JMP _osnotimp
.res		GSREAD-*		; GSREAD - Read a character from a string
		JMP _osnotimp
.res		NVRDCH-*		; NVRDCH - Nonvectored OSRDCH
		JMP _osrdch
.res		NVWRCH-*		; NVWRCH - Nonvectored OSWRCH
		JMP _oswrch
.res		OSFIND-*		; OSFIND - Open or close a file
		JMP _osfind
.res		OSGBPB-*		; OSGBPB - Read or write multiple bytes
		JMP _osgbpb
.res		OSBPUT-*		; OSBPUT - Put a single byte to a stream
		JMP _osbput
.res		OSBGET-*		; OSBGET - Get a single byte from a stream
		JMP _osbget
.res		OSARGS-*		; OSARGS - Read information on open files
		JMP _osargs
.res		OSFILE-*		; OSFILE - Perform actions on whole files
		JMP _osfile
.res		OSRDCH-*		; OSRDCH - Read character from input stream
		JMP _osrdch
.res		OSASCI-*		; OSASCI - Write ASCII character to output stream
		; Output a character from A, adding newline if it's CR
		CMP #13
		BNE OSWRCH
.res		OSNEWL-*		; OSNEWL - Write LF/CR to output stream
		LDA #10
		JSR OSWRCH
.res		OSWRCR-*		; OSWRCR - Write CR to output stream
		LDA #13
.res		OSWRCH-*		; OSWRCH - Write character to output stream
		JMP (wrchv)
.res		OSWORD-*		; OSWORD - Perform actions in control block
		JMP _osword
.res		OSBYTE-*		; OSBYTE - Perform actions in registers
		JMP _osbyte
.res		OSCLI-*			; OSCLI  - Interpret command string
		JMP _oscli

; **************************************************
; 6502 vectors.

.res		NMIV-*			; NMIV   - NMI vector
		.ADDR _nmi_handler
.res		RESETV-*		; RESETV - RESET vector
		.ADDR _reset_handler
.res		IRQV-*			; IRQV   - IRQ vector
		.ADDR _irq_handler
