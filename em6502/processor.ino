/* 6502 registers and helper functions.
 */

/* Get a 16 bit word from memory.
 */
INLINE word getword(word address)
{
    return (word) memread(address) | ((word) memread(address + 1) << 8);
}

INLINE uint32 getword32(word address)
{
    return (word) memread(address) | ((word) memread(address + 1) << 8) |
        ((word) memread(address + 2) << 16) | ((word) memread(address + 3) <<
                                               24);
}

INLINE void putword(word address, word value)
{
	memwrite(address, (byte)(value & 0xFF));
	memwrite(address+1, (byte)((value & 0xFF00) >> 8));
}

INLINE void putword32(word address, uint32 value)
{
	memwrite(address, (byte)(value & 0xFF));
	memwrite(address+1, (byte)((value & 0xFF00) >> 8));
	memwrite(address+2, (byte)((value & 0xFF0000) >> 16));
	memwrite(address+3, (byte)((value & 0xFF000000) >> 24));
}

/* There are a few of places (ADC/SBC/ASL/LSR/ROL/ROR for example)
 * where we operate on SR twice, once to change some flags,
 * then again to change N & Z. The naiive way of doing it 
 * involves masking SR twice. So let's have setnz_mask which
 * can be called in those cases to avoid one masking operation.
 * We can call it from the normal setnz() function.
 */
INLINE void setnz_mask(byte value, byte mask)
{
    SR &= mask;
    if (value & 0x80)
    {
        SR |= bit_N;
    }
    else if (value == 0)
    {
        SR |= bit_Z;
    }
}

/* Given a value, set the N and Z flags appropriately.
 * Used by many op_ functions.
 */
INLINE void setnz(byte value)
{
    setnz_mask(value, 255 ^ bit_N ^ bit_Z);
}

/* Push a value onto the stack
 */
INLINE void push(byte value)
{
    memwrite(0x100 | SP--, value);
}

/* Pop a value off the stack
 */
INLINE byte pop(void)
{
    return memread(0x100 | ++SP);
}

/* Reset the processor.
 */
void reset()
{
    /* Here's how a real 6502 leaves its registers on boot.
     */
    PC = getword(V_RESET) - 1;
    SP = 0;
    SR = 0;
    push(PC >> 8);
    push(PC & 0xFF);
    push(SR | 0b00110000);
}
