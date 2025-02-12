#include "CPU/CPU.h"
#include "MemoryBus.h"

void CPU::Step()
{
	uint8_t opcode = MemoryBus::Read(this->registers.PC);
	switch(opcode)
	{
	case 0x00: // NOP
		this->registers.PC += 1;
		this->cycles += 1;
		break;
	case 0x01: // LD BC, n16
		opcode_LD_r16(this->registers.BC);
		break;
	case 0x02: // LD [BC], A
		opcode_LD_r16_A(this->registers.BC);
		break;
	case 0x03: // INC BC
		opcode_INC_r16(this->registers.BC);
		break;
	case 0x04: // INC B
		opcode_INC_r8(this->registers.B);
		break;
	case 0x05: // DEC B
		opcode_DEC_r8(this->registers.B);
		break;
	case 0x06: // LD B, n8
		opcode_LD_r8_n8(this->registers.B);
		break;
	case 0x07: // RLCA
		opcode_RLCA();
		break;
	case 0x08: // LD [a16], SP
		opcode_LD_n16_r16(this->registers.SP);
		break;
	case 0x09: // ADD HL, BC
		opcode_ADD_hl(this->registers.BC);
		break;
	case 0x0A: // LD A, [BC]
		opcode_LD_r8_r16address(this->registers.A, this->registers.BC);
		break;
	case 0x0B: // DEC BC
		opcode_DEC_r16(this->registers.BC);
		break;
	case 0x0C: // INC C
		opcode_INC_r8(this->registers.C);
		break;
	case 0x0D: // DEC C
		opcode_DEC_r8(this->registers.C);
		break;
	case 0x0E: // LD C, n8
		opcode_LD_r8_n8(this->registers.C);
		break;
	case 0x0F: // RRCA
		opcode_RRCA();
		break;
	case 0x10: // STOP n8
		// TODO
		this->registers.PC += 2;
		break;
	case 0x11: // LD DE n16
		opcode_LD_r16(this->registers.DE);
		break;
	case 0x12: // LD [DE], A
		opcode_LD_r16_A(this->registers.DE);
		break;
	case 0x13: // INC DE
		opcode_INC_r16(this->registers.DE);
		break;
	case 0x14: // INC D
		opcode_INC_r8(this->registers.D);
		break;
	case 0x15: // DEC D
		opcode_DEC_r8(this->registers.D);
		break;
	case 0x16: // LD D, n8
		opcode_LD_r8_n8(this->registers.D);
		break;	
	case 0x17: // RLA
		opcode_RLA();
		break;
	case 0x18: // JR e8
		opcode_JR();
		break;
	case 0x19: // ADD HL, DE
		opcode_ADD_hl(this->registers.DE);
		break;
	case 0x1A: // LD A, [DE]
		opcode_LD_r8_r16address(this->registers.A, this->registers.DE);
		break;
	case 0x1B: // DEC DE
		opcode_DEC_r16(this->registers.DE);
		break;
	case 0x1C: // INC E
		opcode_INC_r8(this->registers.E);
		break;
	case 0x1D: // DEC E
		opcode_DEC_r8(this->registers.E);
		break;
	case 0x1E: // LD E, n8
		opcode_LD_r8_n8(this->registers.E);
		break;
	case 0x1F: // RRA
		opcode_RRA();
		break;
	case 0x20: // JR NZ, e8
		opcode_JR_condition(get_zero_flag() == 0); // NZ = Not zero
		break;
	case 0x21: // LD HL, n16
		opcode_LD_r16(this->registers.HL);
		break;
	case 0x22: // LD [HL+] A
		opcode_LD_r16_A(this->registers.HL);
		this->registers.HL += 1;
		break;
	case 0x23: // INC HL
		opcode_INC_r16(this->registers.HL);
		break;
	case 0x24: // INC H
		opcode_INC_r8(this->registers.H);
		break;
	case 0x25: // DEC H
		opcode_DEC_r8(this->registers.H);
		break;
	case 0x26: // LD H, n8
		opcode_LD_r8_n8(this->registers.H);
		break;
	case 0x27: // DAA
		opcode_DAA();
		break;
	case 0x28: // JR Z, e8
		opcode_JR_condition(get_zero_flag() == 1); // Z = Zero
		break;
	case 0x29: // ADD HL, HL
		opcode_ADD_hl(this->registers.HL);
		break;
	case 0x2A: // LD A, [HL+]
		opcode_LD_r8_r16address(this->registers.A, this->registers.HL);
		this->registers.HL += 1;
		break;
	case 0x2B: // DEC HL
		opcode_DEC_r16(this->registers.HL);
		break;
	case 0x2C: // INC L
		opcode_INC_r8(this->registers.L);
		break;
	case 0x2D: // DEC L
		opcode_DEC_r8(this->registers.L);
		break;
	case 0x2E: // LD L, n8
		opcode_LD_r8_n8(this->registers.L);
		break;
	case 0x2F: // CPL
		opcode_CPL();
		break;
	case 0x30: // JR NC, e8
		opcode_JR_condition(get_carry_flag() == 0); // NC = Not carry
		break;
	case 0x31: // LD SP, n16
		opcode_LD_r16(this->registers.SP);
		break;
	case 0x32: // LD [HL-], A
		opcode_LD_r16_A(this->registers.HL);
		this->registers.HL -= 1;
		break;
	case 0x33: // INC SP
		opcode_INC_r16(this->registers.SP);
		break;
	case 0x34: // INC [HL]
		opcode_INC_hl();
		break;
	case 0x35: // DEC [HL]
		opcode_DEC_hl();
		break;
	case 0x36: // LD [HL], n8
		opcode_LD_hl_n8();
		break;
	case 0x37: // SCF
		opcode_SCF();
		break;
	case 0x38: // Jr C, e8
		opcode_JR_condition(get_carry_flag() == 1); // C = Carry
		break;
	case 0x39: // ADD HL, SP
		opcode_ADD_hl(this->registers.SP);
		break;
	case 0x3A: // LD A, [HL-]
		opcode_LD_r8_r16address(this->registers.A, this->registers.HL);
		this->registers.HL -= 1;
		break;
	case 0x3B: // DEC SP
		opcode_DEC_r16(this->registers.SP);
		break;
	case 0x3C: // INC A
		opcode_INC_r8(this->registers.A);
		break;
	case 0x3D: // DEC A
		opcode_DEC_r8(this->registers.A);
		break;
	case 0x3E: // LD A, n8
		opcode_LD_r8_n8(this->registers.A);
		break;
	case 0x3F: // CCF
		opcode_CCF();
		break;
	case 0x40: // LD B, B
		opcode_LD_r8_r8(this->registers.B, this->registers.B);
		break;
	case 0x41: // LD B, C
		opcode_LD_r8_r8(this->registers.B, this->registers.C);
		break;
	case 0x42: // LD B, D
		opcode_LD_r8_r8(this->registers.B, this->registers.D);
		break;
	case 0x43: // LD B, E
		opcode_LD_r8_r8(this->registers.B, this->registers.E);
		break;
	case 0x44: // LD B, H
		opcode_LD_r8_r8(this->registers.B, this->registers.H);
		break;
	case 0x45: // LD B, L
		opcode_LD_r8_r8(this->registers.B, this->registers.L);
		break;
	case 0x46: // LD B, [HL]
		opcode_LD_r8_r16address(this->registers.B, this->registers.HL);
		break;
	case 0x47: // LD B, A
		opcode_LD_r8_r8(this->registers.B, this->registers.A);
		break;
	case 0x48: // LD C, B
		opcode_LD_r8_r8(this->registers.C, this->registers.B);
		break;
	case 0x49: // LD C, C
		opcode_LD_r8_r8(this->registers.C, this->registers.C);
		break;
	case 0x4A: // LD C, D
		opcode_LD_r8_r8(this->registers.C, this->registers.D);
		break;
	case 0x4B: // LD C, E
		opcode_LD_r8_r8(this->registers.C, this->registers.E);
		break;
	case 0x4C: // LD C, H
		opcode_LD_r8_r8(this->registers.C, this->registers.H);
		break;
	case 0x4D: // LD C, L
		opcode_LD_r8_r8(this->registers.C, this->registers.L);
		break;
	case 0x4E: // LD C, [HL]
		opcode_LD_r8_r16address(this->registers.C, this->registers.HL);
		break;
	case 0x4F: // LD C, A
		opcode_LD_r8_r8(this->registers.C, this->registers.A);
		break;
	case 0x50: // LD D, B
		opcode_LD_r8_r8(this->registers.D, this->registers.B);
		break;
	case 0x51: // LD D, C
		opcode_LD_r8_r8(this->registers.D, this->registers.C);
		break;
	case 0x52: // LD D, D
		opcode_LD_r8_r8(this->registers.D, this->registers.D);
		break;
	case 0x53: // LD D, E
		opcode_LD_r8_r8(this->registers.D, this->registers.E);
		break;
	case 0x54: // LD D, H
		opcode_LD_r8_r8(this->registers.D, this->registers.H);
		break;
	case 0x55: // LD D, L
		opcode_LD_r8_r8(this->registers.D, this->registers.L);
		break;
	case 0x56: // LD D, [HL]
		opcode_LD_r8_r16address(this->registers.D, this->registers.HL);
		break;
	case 0x57: // LD D, A
		opcode_LD_r8_r8(this->registers.D, this->registers.A);
		break;
	case 0x58: // LD E, B
		opcode_LD_r8_r8(this->registers.E, this->registers.B);
		break;
	case 0x59: // LD E, C
		opcode_LD_r8_r8(this->registers.E, this->registers.C);
		break;
	case 0x5A: // LD E, D
		opcode_LD_r8_r8(this->registers.E, this->registers.D);
		break;
	case 0x5B: // LD E, E
		opcode_LD_r8_r8(this->registers.E, this->registers.E);
		break;
	case 0x5C: // LD E, H
		opcode_LD_r8_r8(this->registers.E, this->registers.H);
		break;
	case 0x5D: // LD E, L
		opcode_LD_r8_r8(this->registers.E, this->registers.L);
		break;
	case 0x5E: // LD E, [HL]
		opcode_LD_r8_r16address(this->registers.E, this->registers.HL);
		break;
	case 0x5F: // LD E, A
		opcode_LD_r8_r8(this->registers.E, this->registers.A);
		break;
	case 0x60: // LD H, B
		opcode_LD_r8_r8(this->registers.H, this->registers.B);
		break;
	case 0x61: // LD H, C
		opcode_LD_r8_r8(this->registers.H, this->registers.C);
		break;
	case 0x62: // LD H, D
		opcode_LD_r8_r8(this->registers.H, this->registers.D);
		break;
	case 0x63: // LD H, E
		opcode_LD_r8_r8(this->registers.H, this->registers.E);
		break;
	case 0x64: // LD H, H
		opcode_LD_r8_r8(this->registers.H, this->registers.H);
		break;
	case 0x65: // LD H, L
		opcode_LD_r8_r8(this->registers.H, this->registers.L);
		break;
	case 0x66: // LD H, [HL]
		opcode_LD_r8_r16address(this->registers.H, this->registers.HL);
		break;
	case 0x67: // LD H, A
		opcode_LD_r8_r8(this->registers.H, this->registers.A);
		break;
	case 0x68: // LD L, B
		opcode_LD_r8_r8(this->registers.L, this->registers.B);
		break;
	case 0x69: // LD L, C
		opcode_LD_r8_r8(this->registers.L, this->registers.C);
		break;
	case 0x6A: // LD L, D
		opcode_LD_r8_r8(this->registers.L, this->registers.D);
		break;
	case 0x6B: // LD L, E
		opcode_LD_r8_r8(this->registers.L, this->registers.E);
		break;
	case 0x6C: // LD L, H
		opcode_LD_r8_r8(this->registers.L, this->registers.H);
		break;
	case 0x6D: // LD L, L
		opcode_LD_r8_r8(this->registers.L, this->registers.L);
		break;
	case 0x6E: // LD L, [HL]
		opcode_LD_r8_r16address(this->registers.L, this->registers.HL);
		break;
	case 0x6F: // LD L, A
		opcode_LD_r8_r8(this->registers.L, this->registers.A);
		break;
	case 0x70: // LD [HL], B
		opcode_LD_hl_r8(this->registers.B);
		break;
	case 0x71: // LD [HL], C
		opcode_LD_hl_r8(this->registers.C);
		break;
	case 0x72: // LD [Hl], D
		opcode_LD_hl_r8(this->registers.D);
		break;
	case 0x73: // LD [HL], E
		opcode_LD_hl_r8(this->registers.E);
		break;
	case 0x74: // LD [HL], H
		opcode_LD_hl_r8(this->registers.H);
		break;
	case 0x75: // LD [HL], L
		opcode_LD_hl_r8(this->registers.L);
		break;
	case 0x76: // HALT
		// TODO
		this->registers.PC += 1;
		break;
	case 0x77: // LD [HL], A
		opcode_LD_hl_r8(this->registers.A);
		break;
	case 0x78: // LD A, B
		opcode_LD_r8_r8(this->registers.A, this->registers.B);
		break;
	case 0x79: // LD A, C
		opcode_LD_r8_r8(this->registers.A, this->registers.C);
		break;
	case 0x7A: // LD A, D
		opcode_LD_r8_r8(this->registers.A, this->registers.D);
		break;
	case 0x7B: // LD A, E
		opcode_LD_r8_r8(this->registers.A, this->registers.E);
		break;
	case 0x7C: // LD A, H
		opcode_LD_r8_r8(this->registers.A, this->registers.H);
		break;
	case 0x7D: // LD A, L
		opcode_LD_r8_r8(this->registers.A, this->registers.L);
		break;
	case 0x7E: // LD A, [HL]
		opcode_LD_r8_r16address(this->registers.A, this->registers.HL);
		break;
	case 0x7F: // LD A, A
		opcode_LD_r8_r8(this->registers.A, this->registers.A);
		break;
	case 0x80: // ADD A, B
		opcode_ADD_A_r8(this->registers.B);
		break;
	case 0x81: // ADD A, C
		opcode_ADD_A_r8(this->registers.C);
		break;
	case 0x82: // ADD A, D
		opcode_ADD_A_r8(this->registers.D);
		break;
	case 0x83: // ADD A, E
		opcode_ADD_A_r8(this->registers.E);
		break;
	case 0x84: // ADD A, H
		opcode_ADD_A_r8(this->registers.H);
		break;
	case 0x85: // ADD A, L
		opcode_ADD_A_r8(this->registers.L);
		break;
	case 0x86: // ADD A, [HL]
		opcode_ADD_A_hl();
		break;
	case 0x87: // ADD A, A
		opcode_ADD_A_r8(this->registers.A);
		break;
	case 0x88: // ADC A, B
		opcode_ADC_A_r8(this->registers.B);
		break;
	case 0x89: // ADC A, C
		opcode_ADC_A_r8(this->registers.C);
		break;
	case 0x8A: // ADC A, D
		opcode_ADC_A_r8(this->registers.D);
		break;
	case 0x8B: // ADC A, E
		opcode_ADC_A_r8(this->registers.E);
		break;
	case 0x8C: // ADC A, H
		opcode_ADC_A_r8(this->registers.H);
		break;
	case 0x8D: // ADC A, L
		opcode_ADC_A_r8(this->registers.L);
		break;
	case 0x8E: // ADC A, [HL]
		opcode_ADC_A_hl();
		break;
	case 0x8F: // ADC A, A
		opcode_ADC_A_r8(this->registers.A);
		break;
	case 0x90: // SUB A, B
		opcode_SUB_A_r8(this->registers.B);
		break;
	case 0x91: // SUB A, C
		opcode_SUB_A_r8(this->registers.C);
		break;
	case 0x92: // SUB A, D
		opcode_SUB_A_r8(this->registers.D);
		break;
	case 0x93: // SUB A, E
		opcode_SUB_A_r8(this->registers.E);
		break;
	case 0x94: // SUB A, H
		opcode_SUB_A_r8(this->registers.H);
		break;
	case 0x95: // SUB A, L
		opcode_SUB_A_r8(this->registers.L);
		break;
	case 0x96: // SUB A, [HL]
		opcode_SUB_A_hl();
		break;
	case 0x97: // SUB A, A
		opcode_SUB_A_r8(this->registers.A);
		break;
	case 0x98: // SBC A, B
		opcode_SBC_A_r8(this->registers.B);
		break;
	case 0x99: // SBC A, C
		opcode_SBC_A_r8(this->registers.C);
		break;
	case 0x9A: // SBC A, D
		opcode_SBC_A_r8(this->registers.D);
		break;
	case 0x9B: // SBC A, E
		opcode_SBC_A_r8(this->registers.E);
		break;
	case 0x9C: // SBC A, H
		opcode_SBC_A_r8(this->registers.H);
		break;
	case 0x9D: // SBC A, L
		opcode_SBC_A_r8(this->registers.L);
		break;
	case 0x9E: // SBC A, [HL]
		opcode_SBC_A_hl();
		break;
	case 0x9F: // SBC A, A
		opcode_SBC_A_r8(this->registers.A);
		break;
	case 0xA0: // AND A, B
		opcode_AND_A_r8(this->registers.B);
		break;
	case 0xA1: // AND A, C
		opcode_AND_A_r8(this->registers.C);
		break;
	case 0xA2: // AND A, D
		opcode_AND_A_r8(this->registers.D);
		break;
	case 0xA3: // ANd A, E
		opcode_AND_A_r8(this->registers.E);
		break;
	case 0xA4: // AND A, H
		opcode_AND_A_r8(this->registers.H);
		break;
	case 0xA5: // AND A, L
		opcode_AND_A_r8(this->registers.L);
		break;
	case 0xA6: // AND A, [HL]
		opcode_AND_A_hl();
		break;
	case 0xA7: // AND A, A
		opcode_AND_A_r8(this->registers.A);
		break;
	case 0xA8: // XOR A, B
		opcode_XOR_A_r8(this->registers.B);
		break;
	case 0xA9: // XOR A, C
		opcode_XOR_A_r8(this->registers.C);
		break;
	case 0xAA: // XOR A, D
		opcode_XOR_A_r8(this->registers.D);
		break;
	case 0xAB: // XOR A, E
		opcode_XOR_A_r8(this->registers.E);
		break;
	case 0xAC: // XOR A, H
		opcode_XOR_A_r8(this->registers.H);
		break;
	case 0xAD: // XOR A, L
		opcode_XOR_A_r8(this->registers.L);
		break;
	case 0xAE: // XOR A, [HL]
		opcode_XOR_A_hl();
		break;
	case 0xAF: // XOR A, A
		opcode_XOR_A_r8(this->registers.A);
		break;
	case 0xB0: // OR A, B
		opcode_OR_A_r8(this->registers.B);
		break;
	case 0xB1: // OR A, C
		opcode_OR_A_r8(this->registers.C);
		break;
	case 0xB2: // OR A, D
		opcode_OR_A_r8(this->registers.D);
		break;
	case 0xB3: // OR A, E
		opcode_OR_A_r8(this->registers.E);
		break;
	case 0xB4: // OR A, H
		opcode_OR_A_r8(this->registers.H);
		break;
	case 0xB5: // OR A, L
		opcode_OR_A_r8(this->registers.L);
		break;
	case 0xB6: // OR A, [HL]
		opcode_OR_A_hl();
		break;
	case 0xB7: // OR A, A
		opcode_OR_A_r8(this->registers.A);
		break;
	case 0xB8: // CP A, B
		opcode_CP_A_r8(this->registers.B);
		break;
	case 0xB9: // CP A, C
		opcode_CP_A_r8(this->registers.C);
		break;
	case 0xBA: // CP A, D
		opcode_CP_A_r8(this->registers.D);
		break;
	case 0xBB: // CP A, E
		opcode_CP_A_r8(this->registers.E);
		break;
	case 0xBC: // CP A, H
		opcode_CP_A_r8(this->registers.H);
		break;
	case 0xBD: // CP A, L
		opcode_CP_A_r8(this->registers.L);
		break;
	case 0xBE: // CP A, [HL]
		opcode_CP_A_hl();
		break;
	case 0xBF: // CP A, A
		opcode_CP_A_r8(this->registers.A);
		break;
	case 0xC0: // RET NZ
		opcode_RET_cond(get_zero_flag() == 0);
		break;
	case 0xC1: // POP BC
		opcode_POP_r16(this->registers.BC);
		break;
	case 0xC2: // JP NZ, a16
		opcode_JP_cond(get_zero_flag() == 0);
		break;
	case 0xC3: // JP a16
		opcode_JP_n16();
		break;
	case 0xC4: // CALL NZ, a16
		opcode_CALL_cond(get_zero_flag() == 0);
		break;
	case 0xC5: // PUSH BC
		opcode_PUSH_r16(this->registers.BC);
		break;
	case 0xC6: // ADD A, n8
		opcode_ADD_A_n8();
		break;
	case 0xC7: // RST $00
		opcode_RST(0x00);
		break;
	case 0xC8: // RET Z
		opcode_RET_cond(get_zero_flag() == 1);
		break;
	case 0xC9: // RET
		opcode_RET();
		break;
	case 0xCA: // JP Z, a16
		opcode_JP_cond(get_zero_flag() == 1);
		break;
	case 0xCB:
		// TODO
		std::cout << "Attempted to call CB opcode, but it isn't implemented yet" << std::endl;
		this->registers.PC += 2;
		break;
	case 0xCC: // CALL Z, a16
		opcode_CALL_cond(get_zero_flag() == 1);
		break;
	case 0xCD: // CALL a16
		opcode_CALL();
		break;
	case 0xCE: // ADC A, n8
		opcode_ADC_A_n8();
		break;
	case 0xCF: // RST $08
		opcode_RST(0x08);
		break;
	case 0xD0: // RET NC
		opcode_RET_cond(get_carry_flag() == 0);
		break;
	case 0xD1: // POP DE
		opcode_POP_r16(this->registers.DE);
		break;
	case 0xD2: // JP NC, a16
		opcode_JP_cond(get_carry_flag() == 0);
		break;
	case 0xD3: // invalid
		this->registers.PC += 1;
		break;
	case 0xD4: // CALL NZ, a16
		opcode_CALL_cond(get_zero_flag() == 0);
		break;
	case 0xD5: // PUSH DE
		opcode_PUSH_r16(this->registers.DE);
		break;
	case 0xD6: // SUB A, n8
		opcode_SUB_A_n8();
		break;
	case 0xD7: // RST $10
		opcode_RST(0x10);
		break;
	case 0xD8: // RET C
		opcode_RET_cond(get_carry_flag() == 1);
		break;
	case 0xD9: // RETI
		opcode_RETI();
		break;
	case 0xDA: // JP C, a16
		opcode_JP_cond(get_carry_flag() == 1);
		break;
	case 0xDB: // invalid
		this->registers.PC += 1;
		break;
	case 0xDC: // CALL C, a16
		opcode_CALL_cond(get_carry_flag() == 1);
		break;
	case 0xDD: // invalid
		this->registers.PC += 1;
		break;
	case 0xDE: // SBC A, n8
		opcode_SBC_A_n8();
		break;
	case 0xDF: // RST $18
		opcode_RST(0x18);
		break;
	case 0xE0: // LDH [a8], A
		opcode_LDH_n8_r8(this->registers.A);
		break;
	case 0xE1: // POP HL
		opcode_POP_r16(this->registers.HL);
		break;
	case 0xE2: // LDH [C], A
		opcode_LDH_addr8_r8(this->registers.C, this->registers.A);
		break;
	case 0xE3: // invalid
	case 0xE4: // invalid
		this->registers.PC += 1;
		break;
	case 0xE5: // PUSH HL
		opcode_PUSH_r16(this->registers.HL);
		break;
	case 0xE6: // AND A, n8
		opcode_AND_A_n8();
		break;
	case 0xE7: // RST $20
		opcode_RST(0x20);
		break;
	case 0xE8: // ADD SP, e8
		opcode_ADD_SP_e8();
		break;
	case 0xE9: // JP HL
		opcode_JP_hl();
		break;
	case 0xEA: // LD [a16], A
		opcode_LD_addr16_A();
		break;
	case 0xEB:
	case 0xEC:
	case 0xED: // invalid
		this->registers.PC += 1;
		break;
	case 0xEE: // XOR A, n8
		opcode_XOR_A_n8();
		break;
	case 0xEF: // RST $28
		opcode_RST(0x28);
		break;
	case 0xF0: // LDH A, [a8]
		opcode_LDH_r8_n8(this->registers.A);
		break;
	case 0xF1: // POP AF
		opcode_POP_r16(this->registers.AF);
		break;
	case 0xF2: // LDH A, [C]
		opcode_LDH_r8_addr8(this->registers.A, this->registers.C);
		break;
	case 0xF3: // DI
		opcode_DI();
		break;
	case 0xF4: // invalid
		this->registers.PC += 1;
		break;
	case 0xF5: // PUSH AF
		opcode_PUSH_r16(this->registers.AF);
		break;
	case 0xF6: // OR A, n8
		opcode_OR_A_n8();
		break;
	case 0xF7: // RST $30
		opcode_RST(0x30);
	case 0xF8: // LD HL, SP + e8
		opcode_LD_hl_sp_n8();
		break;
	case 0xF9: // LD SP, HL
		opcode_LD_r16_r16(this->registers.SP, this->registers.HL);
		break;
	case 0xFA: // LD A, [a16]
		opcode_LD_r8_n16(this->registers.A);
		break;
	case 0xFB: // EI
		opcode_EI();
		break;
	case 0xFC:
	case 0xFD: // invalid
		this->registers.PC++;
		break;
	case 0xFE: // CP A, n8
		opcode_CP_A_n8();
		break;
	case 0xFF: // RST 0x38
		opcode_RST(0x38);
		break;
	default:
		std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
		break;
	}
}