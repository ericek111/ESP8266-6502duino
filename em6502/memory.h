#define RAMSIZE     0x8000

byte ram[RAMSIZE];

/* Emulated memory read and write.
 */
byte memread(word address);
void memwrite(word address, byte value);

#include "BASIC.h"
#include "MOS.h"
