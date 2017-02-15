; My emulated machine uses opcode $02 as a coprocessor instruction to 
; call out to the emulator. The byte following the opcode holds an 
; operation code for the coprocessor. The coprocessor may modify A and,
; if it does so, it will set N and Z appropriately.

COP		= $02	; Opcode

COP_DEBUG	= $00	; Enable/disable emulation debug flag, depending on A.
COP_RAMTOP	= $01	; Return the number of pages of RAM, in A.
COP_REBOOT	= $02	; Full reboot of ESP.
COP_ICOUNTER	= $03	; Set or get icounter.
COP_IFLAG	= $04	; Get or clear iflag.

COP_OSFIND	= $05	; Implements OSFIND.
COP_OSFILE	= $06	; Implements OSFILE.
COP_OSBGET	= $07	; Implements OSBGET.
COP_OSBPUT	= $08	; Implements OSBPUT.
COP_OSGBPB	= $09	; Implements OSGBPB.
COP_SENDCHAR	= $0A	; Output character.
COP_GETCHAR	= $0B	; Read character if present.
COP_OSARGS	= $0C	; Implements OSARGS.
