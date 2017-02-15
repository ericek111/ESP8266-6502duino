/* Emulated memory read and write.
 */
byte memread(word address)
{
    /* Read from RAM?
     */
    if (address < RAMSIZE)
    {
        return ram[address];
    }

    /* Read from our MOS image?
     * It's assumed that the MOS extends from MOS_BASE to 0xFFFF - 
     * reasonable, since the IRQ vector is 0xFFFE-0xFFFF.
     */
    if (address >= MOS_BASE)
    {
        return read_rom(address, MOS + (address) - MOS_BASE);
    }

    /* Read from BASIC ROM?
     */
    if ((address >= BASIC_BASE) && (address < BASIC_BASE + sizeof(BASIC)))
    {
        return read_rom(address, BASIC + (address - BASIC_BASE));
    }
    return 0;
}

void memwrite(word address, byte value)
{
    /* The only place where writing can happen is to RAM.
     * We've not got any memory-mapped hardware, and writes
     * to ROM don't work, obviously!
     */
    if (address < RAMSIZE)
    {
        ram[address] = value;
    }
}

/* 
 * Stuff stored in flash can only be accessed when aligned to a 4 byte
 * boundary. So, to access a byte from ROM we have to read the whole
 * word, then extract the relevant byte.
 *
 * Either the CPU is handling caching for us, or flash access is fast. 
 * Implementing a cache represented an overall performance decrease.
 *
 */

INLINE byte read_rom(word addr6502, byte * address)
{
    uint32 mempos = (uint32) address;
    volatile uint32 romword = *((uint32 *) (mempos & ~3));
    return *(((byte *) & romword) + (mempos & 3));
}
