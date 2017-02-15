/* Instruction decoder. This was generated programatically
 * from opcodes.txt. I also tried a tables-based approach, 
 * but a switch() generated faster code (most likely
 * because the switch() becomes a jump table, avoiding
 * using function pointers and allowing inlining to happen.
 */
INLINE void runop(byte op)
{
    switch (op)
    {
        case 0x00:
            op_BRK();
            break;
        case 0x01:
            addr_zp_indexed_indirect();
            op_ORA();
            break;
        case 0x02:
            op_COP();
            break;
        case 0x05:
            addr_zp();
            op_ORA();
            break;
        case 0x06:
            addr_zp();
            op_ASL();
            break;
        case 0x08:
            op_PHP();
            break;
        case 0x09:
            addr_immediate();
            op_ORA();
            break;
        case 0x0a:
            addr_accumulator();
            op_ASL();
            break;
        case 0x0d:
            addr_absolute();
            op_ORA();
            break;
        case 0x0e:
            addr_absolute();
            op_ASL();
            break;
        case 0x10:
            addr_relative();
            op_BPL();
            break;
        case 0x11:
            addr_zp_indirect_indexed();
            op_ORA();
            break;
        case 0x15:
            addr_zp_indexed_x();
            op_ORA();
            break;
        case 0x16:
            addr_zp_indexed_x();
            op_ASL();
            break;
        case 0x18:
            op_CLC();
            break;
        case 0x19:
            addr_absolute_indexed_y();
            op_ORA();
            break;
        case 0x1d:
            addr_absolute_indexed_x();
            op_ORA();
            break;
        case 0x1e:
            addr_absolute_indexed_x();
            op_ASL();
            break;
        case 0x20:
            addr_absolute();
            op_JSR();
            break;
        case 0x21:
            addr_zp_indexed_indirect();
            op_AND();
            break;
        case 0x24:
            addr_zp();
            op_BIT();
            break;
        case 0x25:
            addr_zp();
            op_AND();
            break;
        case 0x26:
            addr_zp();
            op_ROL();
            break;
        case 0x28:
            op_PLP();
            break;
        case 0x29:
            addr_immediate();
            op_AND();
            break;
        case 0x2a:
            addr_accumulator();
            op_ROL();
            break;
        case 0x2c:
            addr_absolute();
            op_BIT();
            break;
        case 0x2d:
            addr_absolute();
            op_AND();
            break;
        case 0x2e:
            addr_absolute();
            op_ROL();
            break;
        case 0x30:
            addr_relative();
            op_BMI();
            break;
        case 0x31:
            addr_zp_indirect_indexed();
            op_AND();
            break;
        case 0x35:
            addr_zp_indexed_x();
            op_AND();
            break;
        case 0x36:
            addr_zp_indexed_x();
            op_ROL();
            break;
        case 0x38:
            op_SEC();
            break;
        case 0x39:
            addr_absolute_indexed_y();
            op_AND();
            break;
        case 0x3d:
            addr_absolute_indexed_x();
            op_AND();
            break;
        case 0x3e:
            addr_absolute_indexed_x();
            op_ROL();
            break;
        case 0x40:
            op_RTI();
            break;
        case 0x41:
            addr_zp_indexed_indirect();
            op_EOR();
            break;
        case 0x45:
            addr_zp();
            op_EOR();
            break;
        case 0x46:
            addr_zp();
            op_LSR();
            break;
        case 0x48:
            op_PHA();
            break;
        case 0x49:
            addr_immediate();
            op_EOR();
            break;
        case 0x4a:
            addr_accumulator();
            op_LSR();
            break;
        case 0x4c:
            addr_absolute();
            op_JMP();
            break;
        case 0x4d:
            addr_absolute();
            op_EOR();
            break;
        case 0x4e:
            addr_absolute();
            op_LSR();
            break;
        case 0x50:
            addr_relative();
            op_BVC();
            break;
        case 0x51:
            addr_zp_indirect_indexed();
            op_EOR();
            break;
        case 0x55:
            addr_zp_indexed_x();
            op_EOR();
            break;
        case 0x56:
            addr_zp_indexed_x();
            op_LSR();
            break;
        case 0x58:
            op_CLI();
            break;
        case 0x59:
            addr_absolute_indexed_y();
            op_EOR();
            break;
        case 0x5d:
            addr_absolute_indexed_x();
            op_EOR();
            break;
        case 0x5e:
            addr_absolute_indexed_x();
            op_LSR();
            break;
        case 0x60:
            op_RTS();
            break;
        case 0x61:
            addr_zp_indexed_indirect();
            op_ADC();
            break;
        case 0x65:
            addr_zp();
            op_ADC();
            break;
        case 0x66:
            addr_zp();
            op_ROR();
            break;
        case 0x68:
            op_PLA();
            break;
        case 0x69:
            addr_immediate();
            op_ADC();
            break;
        case 0x6a:
            addr_accumulator();
            op_ROR();
            break;
        case 0x6c:
            addr_indirect();
            op_JMP();
            break;
        case 0x6d:
            addr_absolute();
            op_ADC();
            break;
        case 0x6e:
            addr_absolute();
            op_ROR();
            break;
        case 0x70:
            addr_relative();
            op_BVS();
            break;
        case 0x71:
            addr_zp_indirect_indexed();
            op_ADC();
            break;
        case 0x75:
            addr_zp_indexed_x();
            op_ADC();
            break;
        case 0x76:
            addr_zp_indexed_x();
            op_ROR();
            break;
        case 0x78:
            op_SEI();
            break;
        case 0x79:
            addr_absolute_indexed_y();
            op_ADC();
            break;
        case 0x7d:
            addr_absolute_indexed_x();
            op_ADC();
            break;
        case 0x7e:
            addr_absolute_indexed_x();
            op_ROR();
            break;
        case 0x81:
            addr_zp_indexed_indirect();
            op_STA();
            break;
        case 0x84:
            addr_zp();
            op_STY();
            break;
        case 0x85:
            addr_zp();
            op_STA();
            break;
        case 0x86:
            addr_zp();
            op_STX();
            break;
        case 0x88:
            op_DEY();
            break;
        case 0x8a:
            op_TXA();
            break;
        case 0x8c:
            addr_absolute();
            op_STY();
            break;
        case 0x8d:
            addr_absolute();
            op_STA();
            break;
        case 0x8e:
            addr_absolute();
            op_STX();
            break;
        case 0x90:
            addr_relative();
            op_BCC();
            break;
        case 0x91:
            addr_zp_indirect_indexed();
            op_STA();
            break;
        case 0x94:
            addr_zp_indexed_x();
            op_STY();
            break;
        case 0x95:
            addr_zp_indexed_x();
            op_STA();
            break;
        case 0x96:
            addr_zp_indexed_y();
            op_STX();
            break;
        case 0x98:
            op_TYA();
            break;
        case 0x99:
            addr_absolute_indexed_y();
            op_STA();
            break;
        case 0x9a:
            op_TXS();
            break;
        case 0x9d:
            addr_absolute_indexed_x();
            op_STA();
            break;
        case 0xa0:
            addr_immediate();
            op_LDY();
            break;
        case 0xa1:
            addr_zp_indexed_indirect();
            op_LDA();
            break;
        case 0xa2:
            addr_immediate();
            op_LDX();
            break;
        case 0xa4:
            addr_zp();
            op_LDY();
            break;
        case 0xa5:
            addr_zp();
            op_LDA();
            break;
        case 0xa6:
            addr_zp();
            op_LDX();
            break;
        case 0xa8:
            op_TAY();
            break;
        case 0xa9:
            addr_immediate();
            op_LDA();
            break;
        case 0xaa:
            op_TAX();
            break;
        case 0xac:
            addr_absolute();
            op_LDY();
            break;
        case 0xad:
            addr_absolute();
            op_LDA();
            break;
        case 0xae:
            addr_absolute();
            op_LDX();
            break;
        case 0xb0:
            addr_relative();
            op_BCS();
            break;
        case 0xb1:
            addr_zp_indirect_indexed();
            op_LDA();
            break;
        case 0xb4:
            addr_zp_indexed_x();
            op_LDY();
            break;
        case 0xb5:
            addr_zp_indexed_x();
            op_LDA();
            break;
        case 0xb6:
            addr_zp_indexed_y();
            op_LDX();
            break;
        case 0xb8:
            op_CLV();
            break;
        case 0xb9:
            addr_absolute_indexed_y();
            op_LDA();
            break;
        case 0xba:
            op_TSX();
            break;
        case 0xbc:
            addr_absolute_indexed_x();
            op_LDY();
            break;
        case 0xbd:
            addr_absolute_indexed_x();
            op_LDA();
            break;
        case 0xbe:
            addr_absolute_indexed_y();
            op_LDX();
            break;
        case 0xc0:
            addr_immediate();
            op_CPY();
            break;
        case 0xc1:
            addr_zp_indexed_indirect();
            op_CMP();
            break;
        case 0xc4:
            addr_zp();
            op_CPY();
            break;
        case 0xc5:
            addr_zp();
            op_CMP();
            break;
        case 0xc6:
            addr_zp();
            op_DEC();
            break;
        case 0xc8:
            op_INY();
            break;
        case 0xc9:
            addr_immediate();
            op_CMP();
            break;
        case 0xca:
            op_DEX();
            break;
        case 0xcc:
            addr_absolute();
            op_CPY();
            break;
        case 0xcd:
            addr_absolute();
            op_CMP();
            break;
        case 0xce:
            addr_absolute();
            op_DEC();
            break;
        case 0xd0:
            addr_relative();
            op_BNE();
            break;
        case 0xd1:
            addr_zp_indirect_indexed();
            op_CMP();
            break;
        case 0xd5:
            addr_zp_indexed_x();
            op_CMP();
            break;
        case 0xd6:
            addr_zp_indexed_x();
            op_DEC();
            break;
        case 0xd8:
            op_CLD();
            break;
        case 0xd9:
            addr_absolute_indexed_y();
            op_CMP();
            break;
        case 0xdd:
            addr_absolute_indexed_x();
            op_CMP();
            break;
        case 0xde:
            addr_absolute_indexed_x();
            op_DEC();
            break;
        case 0xe0:
            addr_immediate();
            op_CPX();
            break;
        case 0xe1:
            addr_zp_indexed_indirect();
            op_SBC();
            break;
        case 0xe4:
            addr_zp();
            op_CPX();
            break;
        case 0xe5:
            addr_zp();
            op_SBC();
            break;
        case 0xe6:
            addr_zp();
            op_INC();
            break;
        case 0xe8:
            op_INX();
            break;
        case 0xe9:
            addr_immediate();
            op_SBC();
            break;
        case 0xea:
            op_NOP();
            break;
        case 0xec:
            addr_absolute();
            op_CPX();
            break;
        case 0xed:
            addr_absolute();
            op_SBC();
            break;
        case 0xee:
            addr_absolute();
            op_INC();
            break;
        case 0xf0:
            addr_relative();
            op_BEQ();
            break;
        case 0xf1:
            addr_zp_indirect_indexed();
            op_SBC();
            break;
        case 0xf5:
            addr_zp_indexed_x();
            op_SBC();
            break;
        case 0xf6:
            addr_zp_indexed_x();
            op_INC();
            break;
        case 0xf8:
            op_SED();
            break;
        case 0xf9:
            addr_absolute_indexed_y();
            op_SBC();
            break;
        case 0xfd:
            addr_absolute_indexed_x();
            op_SBC();
            break;
        case 0xfe:
            addr_absolute_indexed_x();
            op_INC();
            break;
        default:
            op_NOP();
            break;
    }
}
