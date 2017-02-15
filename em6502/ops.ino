/* One function per processor instruction. Prior to the op_ function being 
 * called, an address mode function may have also been called to set 
 * "EA" or "aflag" as necessary.
 */

INLINE void op_ADC(void)
{
    byte oldA = A;
    byte operand = memread(EA);

    /* Do a wider addition and include the carry.
     * N.B. the carry handling assumes that bit_C is the lowest
     * bit in SR.
     */
    word tmp = ((word) operand) + A + (SR & bit_C);

    /* Truncate the result
     */
    A = tmp & 0xFF;

    /* Clear C & V and change N & Z as appropriate.
     * We then set C and V as needed.
     */
    setnz_mask(A, 255 ^ bit_C ^ bit_V ^ bit_N ^ bit_Z);

    /* Carry?
     */
    if (tmp & 0x100)
    {
        SR |= bit_C;
    }

    /* The V flag calculation is just silly!
     */
    if ((oldA ^ A) & (operand ^ A) & 0x80)
    {
        SR |= bit_V;
    }
}

INLINE void op_AND(void)
{
    A &= memread(EA);
    setnz(A);
}

INLINE void op_ASL(void)
{
    if (aflag)
    {
        byte shifted = A << 1;
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (A & 0x80)
        {
            SR |= bit_C;
        }
        A = shifted;
    }
    else
    {
        byte tmp = memread(EA);
        byte shifted = tmp << 1;
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (tmp & 0x80)
        {
            SR |= bit_C;
        }
        memwrite(EA, shifted);
    }
}

INLINE void branch(byte doit)
{
    if (doit)
    {
        PC = EA;
    }
}

INLINE void op_BCC(void)
{
    branch(!(SR & bit_C));
}

INLINE void op_BCS(void)
{
    branch(SR & bit_C);
}

INLINE void op_BEQ(void)
{
    branch(SR & bit_Z);
}

INLINE void op_BNE(void)
{
    branch(!(SR & bit_Z));
}

INLINE void op_BMI(void)
{
    branch(SR & bit_N);
}

INLINE void op_BPL(void)
{
    branch(!(SR & bit_N));
}

INLINE void op_BVC(void)
{
    branch(!(SR & bit_V));
}

INLINE void op_BVS(void)
{
    branch(SR & bit_V);
}

INLINE void op_BRK(void)
{
    PC++;
    PC++;
    push((byte) (PC >> 8));
    push((byte) (PC & 0xFF));
    push(SR | 0b00110000);
    SR |= bit_I;
    PC = getword(V_IRQ) - 1;
}

INLINE void op_BIT(void)
{
    byte tmp = memread(EA);

    SR &= ~(bit_Z | bit_N | bit_V);

    if (!(A & tmp))
    {
        SR |= bit_Z;
    }

    if (tmp & 0x80)
    {
        SR |= bit_N;
    }

    if (tmp & 0x40)
    {
        SR |= bit_V;
    }
}

INLINE void op_CLC(void)
{
    SR &= ~bit_C;
}

INLINE void op_CLD(void)
{
    SR &= ~bit_D;
}

INLINE void op_CLI(void)
{
    SR &= ~bit_I;
}

INLINE void op_CLV(void)
{
    SR &= ~bit_V;
}

INLINE void compare(byte operand)
{
    byte tmp = memread(EA);

    SR &= ~(bit_C | bit_Z | bit_N);

    if (tmp <= operand)
    {
        SR |= bit_C;
    }

    tmp = operand - tmp;

    if (tmp & 0x80)
    {
        SR |= bit_N;
    }
    else if (tmp == 0)
    {
        SR |= bit_Z;
    }
}

INLINE void op_CMP(void)
{
    compare(A);
}

INLINE void op_CPX(void)
{
    compare(X);
}

INLINE void op_CPY(void)
{
    compare(Y);
}

INLINE void op_COP(void)
{
    PC++;
    do_coprocessor_instruction(memread(PC));
    return;
}

INLINE void op_DEC(void)
{
    byte tmp = memread(EA);
    tmp--;
    memwrite(EA, tmp);
    setnz(tmp);
}

INLINE void op_DEX(void)
{
    --X;
    setnz(X);
}

INLINE void op_DEY(void)
{
    --Y;
    setnz(Y);
}

INLINE void op_EOR(void)
{
    A ^= memread(EA);
    setnz(A);
}

INLINE void op_INC(void)
{
    byte tmp = memread(EA);
    tmp++;
    memwrite(EA, tmp);
    setnz(tmp);
}

INLINE void op_INX(void)
{
    ++X;
    setnz(X);
}

INLINE void op_INY(void)
{
    ++Y;
    setnz(Y);
}

INLINE void op_JMP(void)
{
    PC = EA - 1;
}

INLINE void op_JSR(void)
{
    push((byte) (PC >> 8));
    push((byte) (PC & 0xFF));
    PC = EA - 1;
}

INLINE void op_LDA(void)
{
    A = memread(EA);
    setnz(A);
}

INLINE void op_LDX(void)
{
    X = memread(EA);
    setnz(X);
}

INLINE void op_LDY(void)
{
    Y = memread(EA);
    setnz(Y);
}

INLINE void op_LSR(void)
{
    if (aflag)
    {
        byte shifted = A >> 1;
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (A & 1)
        {
            SR |= bit_C;
        }
        A = shifted;
    }
    else
    {
        byte tmp = memread(EA);
        byte shifted = tmp >> 1;
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (tmp & 1)
        {
            SR |= bit_C;
        }
        memwrite(EA, shifted);
    }
}

INLINE void op_NOP(void)
{
    return;
}

INLINE void op_ORA(void)
{
    A |= memread(EA);
    setnz(A);
}

INLINE void op_PHA(void)
{
    push(A);
}

INLINE void op_PHP(void)
{
    push(SR | 0b00110000);
}

INLINE void op_PLA(void)
{
    A = pop();
    setnz(A);
}

INLINE void op_PLP(void)
{
    SR = pop() & 0b11101111;
}

INLINE void op_ROL(void)
{
    if (aflag)
    {
        /* N.B. bit_C is lowest bit.
         */
        byte shifted = (A << 1) | (SR & bit_C);
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (A & 0x80)
        {
            SR |= bit_C;
        }
        A = shifted;
    }
    else
    {
        byte tmp = memread(EA);
        byte shifted = (tmp << 1) | (SR & bit_C);
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (tmp & 0x80)
        {
            SR |= bit_C;
        }
        memwrite(EA, shifted);
    }
}

INLINE void op_ROR(void)
{
    if (aflag)
    {
        byte shifted = A >> 1;
        if (SR & bit_C)
        {
            shifted |= 0x80;
        }
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (A & 1)
        {
            SR |= bit_C;
        }
        A = shifted;
    }
    else
    {
        byte tmp = memread(EA);
        byte shifted = tmp >> 1;
        if (SR & bit_C)
        {
            shifted |= 0x80;
        }
        setnz_mask(shifted, 255 ^ bit_N ^ bit_Z ^ bit_C);
        if (tmp & 1)
        {
            SR |= bit_C;
        }
        memwrite(EA, shifted);
    }
}

INLINE void op_RTI(void)
{
    SR = pop() & 0b11101111;
    PC = pop();
    PC = PC | ((word) pop() << 8);
    PC--;
}

INLINE void op_RTS(void)
{
    PC = pop();
    PC = PC | ((word) pop() << 8);
}

INLINE void op_SBC(void)
{
    byte oldA = A;
    byte operand = memread(EA);

    word tmp = (word) A - (word) operand - (!(SR & bit_C));

    A = tmp;

    /* Clear C & V and change N & Z as appropriate.
     * We then set C and V as needed.
     */
    setnz_mask(A, 255 ^ bit_C ^ bit_V ^ bit_N ^ bit_Z);

    /* Carry?
     */
    if (!(tmp & 0x100))
    {
        SR |= bit_C;
    }

    /* Overflow?
     */
    if ((oldA ^ A) & ((~operand) ^ A) & 0x80)
    {
        SR |= bit_V;
    }
}

INLINE void op_SEC(void)
{
    SR |= bit_C;
}

INLINE void op_SED(void)
{
    SR |= bit_D;
}

INLINE void op_SEI(void)
{
    SR |= bit_I;
}

INLINE void op_STA(void)
{
    memwrite(EA, A);
}

INLINE void op_STX(void)
{
    memwrite(EA, X);
}

INLINE void op_STY(void)
{
    memwrite(EA, Y);
}

INLINE void op_TAX(void)
{
    X = A;
    setnz(X);
}

INLINE void op_TAY(void)
{
    Y = A;
    setnz(Y);
}

INLINE void op_TSX(void)
{
    X = SP;
    setnz(X);
}

INLINE void op_TXA(void)
{
    A = X;
    setnz(A);
}

INLINE void op_TXS(void)
{
    SP = X;
    setnz(SP);
}

INLINE void op_TYA(void)
{
    A = Y;
    setnz(A);
}
