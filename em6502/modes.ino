/* Addressing modes. These set the global variable, EA to the effective 
 * address derived for the mode. There is just one exception, for 
 * ASL A/ROL A/LSR A/ROR A. The function for these sets the flag "aflag" 
 * to signal that the relevant opcode function should use the accumulator 
 * rather than the location identified by EA.
 */

/* Absolute, as in LDA $2000
 */
INLINE void addr_absolute(void)
{
    PC++;
    EA = getword(PC);
    PC++;
}

/* Absolute Indexed (X), as in LDA $2000,X
 */
INLINE void addr_absolute_indexed_x(void)
{
    PC++;
    EA = getword(PC) + X;
    PC++;
}

/* Absolute Indexed (Y), as in LDA $2000,Y
 */
INLINE void addr_absolute_indexed_y(void)
{
    PC++;
    EA = getword(PC) + Y;
    PC++;
}

/* For ASL, ASR, etc.
 */
INLINE void addr_accumulator(void)
{
    aflag = 1;
}

/* Immediate, as in LDA #$2A
 */
INLINE void addr_immediate(void)
{
    PC++;
    EA = PC;
}

/* Indirect, as in JMP ($2000)
 */
INLINE void addr_indirect(void)
{
    PC++;
    EA = getword(PC);
    PC++;
    EA = getword(EA);
}

/* Relative, as in BNE label
 */
INLINE void addr_relative(void)
{
    PC++;
    EA = PC + (signed char) memread(PC);
}

/* Zero page, as in LDA $70
 */
INLINE void addr_zp(void)
{
    PC++;
    EA = memread(PC);
}

/* Zero page indexed (X), as in LDA $70,X
 */
INLINE void addr_zp_indexed_x(void)
{
    PC++;
    EA = (word) (memread(PC) + X);      /* Intentional wrapping here */
}

/* Zero page indexed (Y), as in LDA $70,Y
 */
INLINE void addr_zp_indexed_y(void)
{
    PC++;
    EA = (word) (memread(PC) + Y);      /* Intentional wrapping here */
}

/* Zero page Y indirect indexed, as in LDA ($70),Y
 */
INLINE void addr_zp_indirect_indexed(void)
{
    PC++;
    EA = getword(memread(PC)) + Y;
}

/* Zero page X indexed indirect, as in LDA ($70,X)
 */
INLINE void addr_zp_indexed_indirect(void)
{
    PC++;
    EA = (word) (memread(PC) + X);      /* Intentional wrapping here */
    EA = getword(EA);
}
