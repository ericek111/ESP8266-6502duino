/* 6502 registers and helper functions.
 */

#define V_NMI       0xFFFA
#define V_RESET     0xFFFC
#define V_IRQ       0xFFFE

#define bit_C           0b00000001
#define bit_Z           0b00000010
#define bit_I           0b00000100
#define bit_D           0b00001000
#define bit_B           0b00010000
#define bit_V           0b01000000
#define bit_N           0b10000000

/* Processor registers.
 */
byte A, X, Y;
byte SP, SR;
word PC;

/* Effective address, as decoded by the addr_* functions.
 * One address mode is "A" (as in ROL A) and we need to flag
 * that case in "aflag".
 */
word EA;
bool aflag;

/* Toggled by a coprocessor call.
 */
bool debugflag;

/* Instruction counter - this increments for every
 * 1000 instructions executed.
 */
long icounter;

/* Interrupt flags.
 */
#define IFLAG_SERIAL	(1<<0)
#define IFLAG_TIMER	(1<<1)
volatile byte iflag;

/* Get a 16 bit word from memory.
 */
INLINE word getword(word address);
INLINE uint32 getword32(word address);
INLINE void putword(word address, word value);
INLINE void putword32(word address, uint32 value);

/* There are a few of places (ADC/SBC/ASL/LSR/ROL/ROR for example)
 * where we operate on SR twice, once to change some flags,
 * then again to change N & Z. The naiive way of doing it 
 * involves masking SR twice. So let's have setnz_mask which
 * can be called in those cases to avoid one masking operation.
 * We can call it from the normal setnz() function.
 */
INLINE void setnz_mask(byte value, byte mask);

/* Given a value, set the N and Z flags appropriately.
 * Used by many op_ functions.
 */
INLINE void setnz(byte value);

/* Push a value onto the stack
 */
INLINE void push(byte value);

/* Pop a value off the stack
 */
INLINE byte pop(void);

/* Reset the processor.
 */
void reset();
