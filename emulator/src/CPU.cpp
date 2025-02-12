#include "CPU.h"
#include "MemoryBus.h"

struct CPUContext
{
	CPU::Registers registers;
	uint8_t cycles;
} context;

bool CPU::Initialize() {
	CPU::Reset();
	return true;
}

void CPU::Shutdown()
{
}

void CPU::Reset()
{
	context.registers.AF = 0x01B0;
	context.registers.BC = 0x0013;
	context.registers.DE = 0x00D8;
	context.registers.HL = 0x014D;
	context.registers.SP = 0xFFFE;
	context.registers.PC = 0x0100;
}

void set_zero_flag(bool set) // Z
{
	if(set)
	{
		context.registers.F |= (0x01 << 7);
	}
	else
	{
		context.registers.F &= ~(0x01 << 7);
	}
}

void set_carry_flag(bool set) // C
{
	if(set)
	{
		context.registers.F |= (0x01 << 4);
	}
	else
	{
		context.registers.F &= ~(0x01 << 4);
	}
}

void set_subtraction_flag(bool set) // N
{
	if(set)
	{
		context.registers.F |= (0x01 << 6);
	}
	else
	{
		context.registers.F &= ~(0x01 << 6);
	}
}

void set_half_carry_flag(bool set) // H
{
	if(set)
	{
		context.registers.F |= (0x01 << 5);
	}
	else
	{
		context.registers.F &= ~(0x01 << 5);
	}
}

uint8_t get_carry_flag()
{
	return (context.registers.F >> 4) & 0x01;
}

uint8_t get_zero_flag()
{
	return (context.registers.F >> 7);
}

uint8_t get_subtraction_flag()
{
	return (context.registers.F >> 6) & 0x01;
}

uint8_t get_half_carry_flag()
{
	return (context.registers.F >> 5) & 0x01;
}

void opcode_LD_r8_n8(uint8_t& r8)
{
	r8 = MemoryBus::Read(context.registers.PC + 1);
	context.registers.PC += 2;
	context.cycles += 2;
}

void opcode_LD_r16(uint16_t& r16)
{
	uint8_t low = MemoryBus::Read(context.registers.PC + 1);
	uint8_t high = MemoryBus::Read(context.registers.PC + 2);
	r16 = (high << 8) | low;
	context.registers.PC += 3;
	context.cycles += 3;
}

void opcode_LD_r16_A(uint16_t& r16)
{
	MemoryBus::Write(r16, context.registers.A);
	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_LD_addr16_A()
{
	uint8_t low = MemoryBus::Read(context.registers.PC + 1);
	uint8_t high = MemoryBus::Read(context.registers.PC + 2);
	uint16_t r16 = (high << 8) | low;
	
	opcode_LD_r16_A(r16);

	context.registers.PC += 2;
	context.cycles += 2;
}

void opcode_LD_r8_r16address(uint8_t& r8, uint16_t r16address)
{
	r8 = MemoryBus::Read(r16address);
	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_LD_r8_n16(uint8_t& r8)
{
	uint8_t low = MemoryBus::Read(context.registers.PC + 1);
	uint8_t high = MemoryBus::Read(context.registers.PC + 2);

	uint16_t address = (high << 8) | low; 
	opcode_LD_r8_r16address(r8, address);

	context.registers.PC += 2;
	context.cycles += 2;
}

void opcode_LD_n16_r16(uint16_t& r16)
{
	uint8_t low = MemoryBus::Read(context.registers.PC + 1);
	uint8_t high = MemoryBus::Read(context.registers.PC + 2);

	uint16_t address = (high << 8) | low; 

	MemoryBus::Write(address, r16 & 0x00FF);
	MemoryBus::Write(address + 1, r16 >> 8);

	context.registers.PC += 3;
	context.cycles += 5;
}

void opcode_LD_r8_r8(uint8_t& r8_0, uint8_t& r8_1)
{
	r8_0 = r8_1;

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_LD_r16_r16(uint16_t& r8_0, uint16_t& r8_1)
{
	r8_0 = r8_1;

	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_LD_hl_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	MemoryBus::Write(context.registers.HL, n8);

	context.registers.PC += 2;
	context.cycles += 3;
}

void opcode_LD_hl_sp_n8()
{
	int8_t e8 = static_cast<int8_t>(MemoryBus::Read(context.registers.PC + 1));
	MemoryBus::Write(context.registers.HL, context.registers.SP + e8);
	
	set_carry_flag((context.registers.SP & 0xFF) + e8 > 0xFF);
	set_half_carry_flag((context.registers.SP & 0x0F) + e8 > 0x0F);

	set_zero_flag(false);
	set_subtraction_flag(false);

	context.registers.PC += 2;
	context.cycles += 3;
}

void opcode_LD_hl_r8(uint8_t& r8)
{
	MemoryBus::Write(context.registers.HL, r8);

	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_LDH_addr8_r8(uint8_t& addr, uint8_t& r8)
{
	MemoryBus::Write(r8 + 0xFF00, r8);

	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_LDH_r8_addr8(uint8_t& r8, uint8_t addr8)
{
	r8 = MemoryBus::Read(addr8 + 0xFF00);

	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_LDH_n8_r8(uint8_t& r8)
{
	uint8_t addr = MemoryBus::Read(context.registers.PC + 1);
	opcode_LDH_addr8_r8(addr, r8);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_LDH_r8_n8(uint8_t& r8)
{
	uint8_t addr = MemoryBus::Read(context.registers.PC + 1);
	opcode_LDH_r8_addr8(r8, addr);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_INC_r16(uint16_t& r16)
{
	r16 += 1;
	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_INC_r8(uint8_t& r8)
{
	set_half_carry_flag((r8 & 0x0F) == 0x0F);

	r8 += 1;

	set_subtraction_flag(false);
	set_zero_flag(r8 == 0);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_INC_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);

	set_half_carry_flag((value & 0x0F) == 0x0F);

	value += 1;

	set_subtraction_flag(false);
	set_zero_flag(value == 0);

	MemoryBus::Write(context.registers.HL, value);

	context.registers.PC += 1;
	context.cycles += 3;
}

void opcode_DEC_r8(uint8_t& r8)
{
	set_half_carry_flag((r8 & 0x0F) == 0x00);

	r8 -= 1;

	set_subtraction_flag(true);
	set_zero_flag(r8 == 0);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_DEC_r16(uint16_t& r16)
{
	r16 -= 1;

	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_DEC_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);

	set_half_carry_flag((value & 0x0F) == 0x00);

	value -= 1;

	set_subtraction_flag(true);
	set_zero_flag(value == 0);

	MemoryBus::Write(context.registers.HL, value);

	context.registers.PC += 1;
	context.cycles += 3;
}

void opcode_ADD_hl(uint16_t& r16)
{
	set_carry_flag(context.registers.HL + r16 > 0xFFFF);
	set_half_carry_flag(context.registers.HL & 0x0FFF + r16 > 0x0FFF);
	set_zero_flag(false);

	context.registers.HL += r16;

	context.registers.PC += 1;
	context.cycles += 2;
}

void opcode_ADD_A_r8(uint8_t& r8)
{
	set_carry_flag(context.registers.A + r8 > 0xFF);
	set_half_carry_flag((context.registers.A & 0x0F) + r8 > 0x0F);

	context.registers.A += r8;

	set_zero_flag(context.registers.A == 0);
	set_subtraction_flag(false);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_ADD_SP_e8()
{
	int8_t e8 = static_cast<int8_t>(MemoryBus::Read(context.registers.PC + 1));
	
	set_carry_flag((context.registers.SP & 0xFF) + e8 > 0xFF);
	set_half_carry_flag((context.registers.SP & 0x0F) + e8 > 0x0F);

	context.registers.SP += e8;

	set_zero_flag(false);
	set_subtraction_flag(false);

	context.registers.PC += 2;
	context.cycles += 4;
}

void opcode_ADD_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_ADD_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_ADD_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_ADD_A_r8(value);
	context.cycles += 1;
}

void opcode_ADC_A_r8(uint8_t& r8)
{
	uint8_t carry = get_carry_flag();
	uint8_t val = r8 + carry;

	opcode_ADD_A_r8(val);
}

void opcode_ADC_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_ADC_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_ADC_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_ADC_A_r8(value);
	context.cycles += 1;
}

void opcode_SUB_A_r8(uint8_t& r8)
{
	set_carry_flag(context.registers.A < r8);
	set_half_carry_flag((context.registers.A & 0x0F) < r8 & 0x0F);

	context.registers.A -= r8;

	set_zero_flag(context.registers.A == 0);
	set_subtraction_flag(true);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_SUB_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_SUB_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_SUB_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_SUB_A_r8(value);
	context.cycles += 1;
}

void opcode_SBC_A_r8(uint8_t& r8)
{
	uint8_t carry = get_carry_flag();
	uint8_t val = r8 + carry;

	opcode_SUB_A_r8(val);
}

void opcode_SBC_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_SBC_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_SBC_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_SBC_A_r8(value);
	context.cycles += 1;
}

void opcode_AND_A_r8(uint8_t& r8)
{
	context.registers.A &= r8;

	set_zero_flag(context.registers.A == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(true);
	set_carry_flag(false);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_AND_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_AND_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_AND_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_AND_A_r8(value);
	context.cycles += 1;
}

void opcode_XOR_A_r8(uint8_t& r8)
{
	context.registers.A ^= r8;

	set_zero_flag(context.registers.A == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(false);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_XOR_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_XOR_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_XOR_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_XOR_A_r8(value);
	context.cycles += 1;
}

void opcode_OR_A_r8(uint8_t& r8)
{
	context.registers.A |= r8;

	set_zero_flag(context.registers.A == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(false);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_OR_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_OR_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_OR_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_OR_A_r8(value);
	context.cycles += 1;
}

void opcode_CP_A_r8(uint8_t& r8)
{
	uint8_t result = context.registers.A - r8;

	set_zero_flag(result == 0);
	set_subtraction_flag(true);
	set_half_carry_flag((context.registers.A & 0x0F) < (r8 & 0x0F));
	set_carry_flag(context.registers.A < r8);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_CP_A_n8()
{
	uint8_t n8 = MemoryBus::Read(context.registers.PC + 1);
	opcode_CP_A_r8(n8);
	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_CP_A_hl()
{
	uint8_t value = MemoryBus::Read(context.registers.HL);
	opcode_CP_A_r8(value);
	context.cycles += 1;
}

void opcode_RLCA()
{
	uint8_t carry = (context.registers.A & 0x80) >> 7;
	context.registers.A = (context.registers.A << 1) | carry;

	set_zero_flag(0);
	set_half_carry_flag(0);
	set_subtraction_flag(0);
	set_carry_flag(carry);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_RRCA()
{
	uint8_t carry = (context.registers.A & 0x01);
	context.registers.A = (context.registers.A >> 1) | (carry << 7);

	set_zero_flag(0);
	set_half_carry_flag(0);
	set_subtraction_flag(0);
	set_carry_flag(carry);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_RLA()
{
	uint8_t carry = get_carry_flag();
	set_carry_flag((context.registers.A & 0x80) >> 7);
	set_zero_flag(false);
	set_subtraction_flag(false);
	set_half_carry_flag(false);

	context.registers.A = (context.registers.A << 1) | carry;

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_RRA()
{
	uint8_t carry = get_carry_flag();
	set_carry_flag((context.registers.A & 0x01));
	set_zero_flag(false);
	set_subtraction_flag(false);
	set_half_carry_flag(false);

	context.registers.A = (context.registers.A >> 1) | (carry << 7);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_JR()
{
	int8_t offset = static_cast<int8_t>(MemoryBus::Read(context.registers.PC + 1));

	context.registers.PC += 1 + offset;
	context.cycles += 3;
}

void opcode_JR_condition(bool condition)
{
	if(condition)
	{
		opcode_JR();
	}
	else
	{
		context.registers.PC += 2;
		context.cycles += 2;
	}
}

void opcode_DAA()
{
	uint8_t adjustment = 0;
	if(get_subtraction_flag())
	{
		if(get_half_carry_flag())
		{
			adjustment += 0x06;
		}

		if(get_carry_flag())
		{
			adjustment += 0x60;
		}

		context.registers.A -= adjustment;
	}
	else
	{
		if(get_half_carry_flag() || context.registers.A & 0x0F > 0x09)
		{
			adjustment += 0x06;
		}

		if(get_carry_flag() || context.registers.A > 0x99)
		{
			adjustment += 0x60;
			set_carry_flag(true);
		}

		context.registers.A += adjustment;
	}

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_CPL()
{
	context.registers.A = ~context.registers.A;

	set_subtraction_flag(true);
	set_half_carry_flag(true);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_SCF()
{
	set_carry_flag(true);
	set_half_carry_flag(false);
	set_subtraction_flag(false);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_CCF()
{
	set_carry_flag(!get_carry_flag());
	set_half_carry_flag(false);
	set_subtraction_flag(false);

	context.registers.PC += 1;
	context.cycles += 1;
}

void opcode_POP_r16(uint16_t& r16)
{
	uint8_t low = MemoryBus::Read(context.registers.SP);
	context.registers.SP++;
	uint8_t high = MemoryBus::Read(context.registers.SP);
	context.registers.SP++;

	r16 = (high << 8) | low;

	context.registers.PC += 1;
	context.cycles += 3;
}

void opcode_PUSH_r16(uint16_t& r16)
{
	context.registers.SP--;
	MemoryBus::Write(context.registers.SP, r16 >> 8);
	context.registers.SP--;
	MemoryBus::Write(context.registers.SP, (uint8_t)(r16 & 0x00FF));

	context.registers.PC += 1;
	context.cycles += 4;
}

void opcode_RET()
{
	opcode_POP_r16(context.registers.PC);
	context.registers.PC -= 1; // I do not know if this should be here 
	context.cycles += 1;
}

void opcode_RET_cond(uint8_t cond)
{
	if(cond)
	{
		opcode_RET();
		context.cycles += 1;
	}
	else
	{
		context.registers.PC++;
		context.cycles += 2;
	}
}

void opcode_JP_r16(uint16_t& r16)
{
	context.registers.PC = r16;
	context.cycles += 4;
}

void opcode_JP_n16()
{
	uint8_t low = MemoryBus::Read(context.registers.PC + 1);
	uint8_t high = MemoryBus::Read(context.registers.PC + 2);

	uint16_t value = (high << 8) | low;
	opcode_JP_r16(value);
}

void opcode_JP_cond(uint8_t cond)
{
	if(cond)
	{
		opcode_JP_n16();
	}
	else
	{
		context.registers.PC += 3;
		context.cycles += 4;
	}
}

void opcode_JP_hl()
{
	context.registers.PC = context.registers.HL;
	context.cycles += 1;
}

void opcode_CALL()
{
	uint8_t low = MemoryBus::Read(context.registers.PC + 1);
	uint8_t high = MemoryBus::Read(context.registers.PC + 2);
	context.registers.PC += 2;

	uint16_t value = (high << 8) | low;

	opcode_PUSH_r16(value);
	opcode_JP_r16(value);

	context.cycles -= 2;
}

void opcode_CALL_cond(uint8_t cond)
{
	if(cond)
	{
		opcode_CALL();
	}
	else
	{
		context.registers.PC += 3;
		context.cycles += 3;
	}
}

void opcode_RST(uint16_t address)
{
	opcode_PUSH_r16(address);
	opcode_JP_r16(address);

	context.cycles -= 4;
}

void opcode_EI()
{
	// TODO
}

void opcode_DI()
{
	// TODO
}

void opcode_RETI()
{
	// TODO
	opcode_EI();
	opcode_RET();
}

void CPU::Step()
{
	uint8_t opcode = MemoryBus::Read(context.registers.PC);
	switch(opcode)
	{
	case 0x00: // NOP
		context.registers.PC += 1;
		context.cycles += 1;
		break;
	case 0x01: // LD BC, n16
		opcode_LD_r16(context.registers.BC);
		break;
	case 0x02: // LD [BC], A
		opcode_LD_r16_A(context.registers.BC);
		break;
	case 0x03: // INC BC
		opcode_INC_r16(context.registers.BC);
		break;
	case 0x04: // INC B
		opcode_INC_r8(context.registers.B);
		break;
	case 0x05: // DEC B
		opcode_DEC_r8(context.registers.B);
		break;
	case 0x06: // LD B, n8
		opcode_LD_r8_n8(context.registers.B);
		break;
	case 0x07: // RLCA
		opcode_RLCA();
		break;
	case 0x08: // LD [a16], SP
		opcode_LD_n16_r16(context.registers.SP);
		break;
	case 0x09: // ADD HL, BC
		opcode_ADD_hl(context.registers.BC);
		break;
	case 0x0A: // LD A, [BC]
		opcode_LD_r8_r16address(context.registers.A, context.registers.BC);
		break;
	case 0x0B: // DEC BC
		opcode_DEC_r16(context.registers.BC);
		break;
	case 0x0C: // INC C
		opcode_INC_r8(context.registers.C);
		break;
	case 0x0D: // DEC C
		opcode_DEC_r8(context.registers.C);
		break;
	case 0x0E: // LD C, n8
		opcode_LD_r8_n8(context.registers.C);
		break;
	case 0x0F: // RRCA
		opcode_RRCA();
		break;
	case 0x10: // STOP n8
		// TODO
		context.registers.PC += 2;
		std::cout << "Attempted to call STOP n8 opcode, but it isn't implemented yet" << std::endl;
		break;
	case 0x11: // LD DE n16
		opcode_LD_r16(context.registers.DE);
		break;
	case 0x12: // LD [DE], A
		opcode_LD_r16_A(context.registers.DE);
		break;
	case 0x13: // INC DE
		opcode_INC_r16(context.registers.DE);
		break;
	case 0x14: // INC D
		opcode_INC_r8(context.registers.D);
		break;
	case 0x15: // DEC D
		opcode_DEC_r8(context.registers.D);
		break;
	case 0x16: // LD D, n8
		opcode_LD_r8_n8(context.registers.D);
		break;	
	case 0x17: // RLA
		opcode_RLA();
		break;
	case 0x18: // JR e8
		opcode_JR();
		break;
	case 0x19: // ADD HL, DE
		opcode_ADD_hl(context.registers.DE);
		break;
	case 0x1A: // LD A, [DE]
		opcode_LD_r8_r16address(context.registers.A, context.registers.DE);
		break;
	case 0x1B: // DEC DE
		opcode_DEC_r16(context.registers.DE);
		break;
	case 0x1C: // INC E
		opcode_INC_r8(context.registers.E);
		break;
	case 0x1D: // DEC E
		opcode_DEC_r8(context.registers.E);
		break;
	case 0x1E: // LD E, n8
		opcode_LD_r8_n8(context.registers.E);
		break;
	case 0x1F: // RRA
		opcode_RRA();
		break;
	case 0x20: // JR NZ, e8
		opcode_JR_condition(get_zero_flag() == 0); // NZ = Not zero
		break;
	case 0x21: // LD HL, n16
		opcode_LD_r16(context.registers.HL);
		break;
	case 0x22: // LD [HL+] A
		opcode_LD_r16_A(context.registers.HL);
		context.registers.HL += 1;
		break;
	case 0x23: // INC HL
		opcode_INC_r16(context.registers.HL);
		break;
	case 0x24: // INC H
		opcode_INC_r8(context.registers.H);
		break;
	case 0x25: // DEC H
		opcode_DEC_r8(context.registers.H);
		break;
	case 0x26: // LD H, n8
		opcode_LD_r8_n8(context.registers.H);
		break;
	case 0x27: // DAA
		opcode_DAA();
		break;
	case 0x28: // JR Z, e8
		opcode_JR_condition(get_zero_flag() == 1); // Z = Zero
		break;
	case 0x29: // ADD HL, HL
		opcode_ADD_hl(context.registers.HL);
		break;
	case 0x2A: // LD A, [HL+]
		opcode_LD_r8_r16address(context.registers.A, context.registers.HL);
		context.registers.HL += 1;
		break;
	case 0x2B: // DEC HL
		opcode_DEC_r16(context.registers.HL);
		break;
	case 0x2C: // INC L
		opcode_INC_r8(context.registers.L);
		break;
	case 0x2D: // DEC L
		opcode_DEC_r8(context.registers.L);
		break;
	case 0x2E: // LD L, n8
		opcode_LD_r8_n8(context.registers.L);
		break;
	case 0x2F: // CPL
		opcode_CPL();
		break;
	case 0x30: // JR NC, e8
		opcode_JR_condition(get_carry_flag() == 0); // NC = Not carry
		break;
	case 0x31: // LD SP, n16
		opcode_LD_r16(context.registers.SP);
		break;
	case 0x32: // LD [HL-], A
		opcode_LD_r16_A(context.registers.HL);
		context.registers.HL -= 1;
		break;
	case 0x33: // INC SP
		opcode_INC_r16(context.registers.SP);
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
		opcode_ADD_hl(context.registers.SP);
		break;
	case 0x3A: // LD A, [HL-]
		opcode_LD_r8_r16address(context.registers.A, context.registers.HL);
		context.registers.HL -= 1;
		break;
	case 0x3B: // DEC SP
		opcode_DEC_r16(context.registers.SP);
		break;
	case 0x3C: // INC A
		opcode_INC_r8(context.registers.A);
		break;
	case 0x3D: // DEC A
		opcode_DEC_r8(context.registers.A);
		break;
	case 0x3E: // LD A, n8
		opcode_LD_r8_n8(context.registers.A);
		break;
	case 0x3F: // CCF
		opcode_CCF();
		break;
	case 0x40: // LD B, B
		opcode_LD_r8_r8(context.registers.B, context.registers.B);
		break;
	case 0x41: // LD B, C
		opcode_LD_r8_r8(context.registers.B, context.registers.C);
		break;
	case 0x42: // LD B, D
		opcode_LD_r8_r8(context.registers.B, context.registers.D);
		break;
	case 0x43: // LD B, E
		opcode_LD_r8_r8(context.registers.B, context.registers.E);
		break;
	case 0x44: // LD B, H
		opcode_LD_r8_r8(context.registers.B, context.registers.H);
		break;
	case 0x45: // LD B, L
		opcode_LD_r8_r8(context.registers.B, context.registers.L);
		break;
	case 0x46: // LD B, [HL]
		opcode_LD_r8_r16address(context.registers.B, context.registers.HL);
		break;
	case 0x47: // LD B, A
		opcode_LD_r8_r8(context.registers.B, context.registers.A);
		break;
	case 0x48: // LD C, B
		opcode_LD_r8_r8(context.registers.C, context.registers.B);
		break;
	case 0x49: // LD C, C
		opcode_LD_r8_r8(context.registers.C, context.registers.C);
		break;
	case 0x4A: // LD C, D
		opcode_LD_r8_r8(context.registers.C, context.registers.D);
		break;
	case 0x4B: // LD C, E
		opcode_LD_r8_r8(context.registers.C, context.registers.E);
		break;
	case 0x4C: // LD C, H
		opcode_LD_r8_r8(context.registers.C, context.registers.H);
		break;
	case 0x4D: // LD C, L
		opcode_LD_r8_r8(context.registers.C, context.registers.L);
		break;
	case 0x4E: // LD C, [HL]
		opcode_LD_r8_r16address(context.registers.C, context.registers.HL);
		break;
	case 0x4F: // LD C, A
		opcode_LD_r8_r8(context.registers.C, context.registers.A);
		break;
	case 0x50: // LD D, B
		opcode_LD_r8_r8(context.registers.D, context.registers.B);
		break;
	case 0x51: // LD D, C
		opcode_LD_r8_r8(context.registers.D, context.registers.C);
		break;
	case 0x52: // LD D, D
		opcode_LD_r8_r8(context.registers.D, context.registers.D);
		break;
	case 0x53: // LD D, E
		opcode_LD_r8_r8(context.registers.D, context.registers.E);
		break;
	case 0x54: // LD D, H
		opcode_LD_r8_r8(context.registers.D, context.registers.H);
		break;
	case 0x55: // LD D, L
		opcode_LD_r8_r8(context.registers.D, context.registers.L);
		break;
	case 0x56: // LD D, [HL]
		opcode_LD_r8_r16address(context.registers.D, context.registers.HL);
		break;
	case 0x57: // LD D, A
		opcode_LD_r8_r8(context.registers.D, context.registers.A);
		break;
	case 0x58: // LD E, B
		opcode_LD_r8_r8(context.registers.E, context.registers.B);
		break;
	case 0x59: // LD E, C
		opcode_LD_r8_r8(context.registers.E, context.registers.C);
		break;
	case 0x5A: // LD E, D
		opcode_LD_r8_r8(context.registers.E, context.registers.D);
		break;
	case 0x5B: // LD E, E
		opcode_LD_r8_r8(context.registers.E, context.registers.E);
		break;
	case 0x5C: // LD E, H
		opcode_LD_r8_r8(context.registers.E, context.registers.H);
		break;
	case 0x5D: // LD E, L
		opcode_LD_r8_r8(context.registers.E, context.registers.L);
		break;
	case 0x5E: // LD E, [HL]
		opcode_LD_r8_r16address(context.registers.E, context.registers.HL);
		break;
	case 0x5F: // LD E, A
		opcode_LD_r8_r8(context.registers.E, context.registers.A);
		break;
	case 0x60: // LD H, B
		opcode_LD_r8_r8(context.registers.H, context.registers.B);
		break;
	case 0x61: // LD H, C
		opcode_LD_r8_r8(context.registers.H, context.registers.C);
		break;
	case 0x62: // LD H, D
		opcode_LD_r8_r8(context.registers.H, context.registers.D);
		break;
	case 0x63: // LD H, E
		opcode_LD_r8_r8(context.registers.H, context.registers.E);
		break;
	case 0x64: // LD H, H
		opcode_LD_r8_r8(context.registers.H, context.registers.H);
		break;
	case 0x65: // LD H, L
		opcode_LD_r8_r8(context.registers.H, context.registers.L);
		break;
	case 0x66: // LD H, [HL]
		opcode_LD_r8_r16address(context.registers.H, context.registers.HL);
		break;
	case 0x67: // LD H, A
		opcode_LD_r8_r8(context.registers.H, context.registers.A);
		break;
	case 0x68: // LD L, B
		opcode_LD_r8_r8(context.registers.L, context.registers.B);
		break;
	case 0x69: // LD L, C
		opcode_LD_r8_r8(context.registers.L, context.registers.C);
		break;
	case 0x6A: // LD L, D
		opcode_LD_r8_r8(context.registers.L, context.registers.D);
		break;
	case 0x6B: // LD L, E
		opcode_LD_r8_r8(context.registers.L, context.registers.E);
		break;
	case 0x6C: // LD L, H
		opcode_LD_r8_r8(context.registers.L, context.registers.H);
		break;
	case 0x6D: // LD L, L
		opcode_LD_r8_r8(context.registers.L, context.registers.L);
		break;
	case 0x6E: // LD L, [HL]
		opcode_LD_r8_r16address(context.registers.L, context.registers.HL);
		break;
	case 0x6F: // LD L, A
		opcode_LD_r8_r8(context.registers.L, context.registers.A);
		break;
	case 0x70: // LD [HL], B
		opcode_LD_hl_r8(context.registers.B);
		break;
	case 0x71: // LD [HL], C
		opcode_LD_hl_r8(context.registers.C);
		break;
	case 0x72: // LD [Hl], D
		opcode_LD_hl_r8(context.registers.D);
		break;
	case 0x73: // LD [HL], E
		opcode_LD_hl_r8(context.registers.E);
		break;
	case 0x74: // LD [HL], H
		opcode_LD_hl_r8(context.registers.H);
		break;
	case 0x75: // LD [HL], L
		opcode_LD_hl_r8(context.registers.L);
		break;
	case 0x76: // HALT
		// TODO
		context.registers.PC += 1;
		break;
	case 0x77: // LD [HL], A
		opcode_LD_hl_r8(context.registers.A);
		break;
	case 0x78: // LD A, B
		opcode_LD_r8_r8(context.registers.A, context.registers.B);
		break;
	case 0x79: // LD A, C
		opcode_LD_r8_r8(context.registers.A, context.registers.C);
		break;
	case 0x7A: // LD A, D
		opcode_LD_r8_r8(context.registers.A, context.registers.D);
		break;
	case 0x7B: // LD A, E
		opcode_LD_r8_r8(context.registers.A, context.registers.E);
		break;
	case 0x7C: // LD A, H
		opcode_LD_r8_r8(context.registers.A, context.registers.H);
		break;
	case 0x7D: // LD A, L
		opcode_LD_r8_r8(context.registers.A, context.registers.L);
		break;
	case 0x7E: // LD A, [HL]
		opcode_LD_r8_r16address(context.registers.A, context.registers.HL);
		break;
	case 0x7F: // LD A, A
		opcode_LD_r8_r8(context.registers.A, context.registers.A);
		break;
	case 0x80: // ADD A, B
		opcode_ADD_A_r8(context.registers.B);
		break;
	case 0x81: // ADD A, C
		opcode_ADD_A_r8(context.registers.C);
		break;
	case 0x82: // ADD A, D
		opcode_ADD_A_r8(context.registers.D);
		break;
	case 0x83: // ADD A, E
		opcode_ADD_A_r8(context.registers.E);
		break;
	case 0x84: // ADD A, H
		opcode_ADD_A_r8(context.registers.H);
		break;
	case 0x85: // ADD A, L
		opcode_ADD_A_r8(context.registers.L);
		break;
	case 0x86: // ADD A, [HL]
		opcode_ADD_A_hl();
		break;
	case 0x87: // ADD A, A
		opcode_ADD_A_r8(context.registers.A);
		break;
	case 0x88: // ADC A, B
		opcode_ADC_A_r8(context.registers.B);
		break;
	case 0x89: // ADC A, C
		opcode_ADC_A_r8(context.registers.C);
		break;
	case 0x8A: // ADC A, D
		opcode_ADC_A_r8(context.registers.D);
		break;
	case 0x8B: // ADC A, E
		opcode_ADC_A_r8(context.registers.E);
		break;
	case 0x8C: // ADC A, H
		opcode_ADC_A_r8(context.registers.H);
		break;
	case 0x8D: // ADC A, L
		opcode_ADC_A_r8(context.registers.L);
		break;
	case 0x8E: // ADC A, [HL]
		opcode_ADC_A_hl();
		break;
	case 0x8F: // ADC A, A
		opcode_ADC_A_r8(context.registers.A);
		break;
	case 0x90: // SUB A, B
		opcode_SUB_A_r8(context.registers.B);
		break;
	case 0x91: // SUB A, C
		opcode_SUB_A_r8(context.registers.C);
		break;
	case 0x92: // SUB A, D
		opcode_SUB_A_r8(context.registers.D);
		break;
	case 0x93: // SUB A, E
		opcode_SUB_A_r8(context.registers.E);
		break;
	case 0x94: // SUB A, H
		opcode_SUB_A_r8(context.registers.H);
		break;
	case 0x95: // SUB A, L
		opcode_SUB_A_r8(context.registers.L);
		break;
	case 0x96: // SUB A, [HL]
		opcode_SUB_A_hl();
		break;
	case 0x97: // SUB A, A
		opcode_SUB_A_r8(context.registers.A);
		break;
	case 0x98: // SBC A, B
		opcode_SBC_A_r8(context.registers.B);
		break;
	case 0x99: // SBC A, C
		opcode_SBC_A_r8(context.registers.C);
		break;
	case 0x9A: // SBC A, D
		opcode_SBC_A_r8(context.registers.D);
		break;
	case 0x9B: // SBC A, E
		opcode_SBC_A_r8(context.registers.E);
		break;
	case 0x9C: // SBC A, H
		opcode_SBC_A_r8(context.registers.H);
		break;
	case 0x9D: // SBC A, L
		opcode_SBC_A_r8(context.registers.L);
		break;
	case 0x9E: // SBC A, [HL]
		opcode_SBC_A_hl();
		break;
	case 0x9F: // SBC A, A
		opcode_SBC_A_r8(context.registers.A);
		break;
	case 0xA0: // AND A, B
		opcode_AND_A_r8(context.registers.B);
		break;
	case 0xA1: // AND A, C
		opcode_AND_A_r8(context.registers.C);
		break;
	case 0xA2: // AND A, D
		opcode_AND_A_r8(context.registers.D);
		break;
	case 0xA3: // ANd A, E
		opcode_AND_A_r8(context.registers.E);
		break;
	case 0xA4: // AND A, H
		opcode_AND_A_r8(context.registers.H);
		break;
	case 0xA5: // AND A, L
		opcode_AND_A_r8(context.registers.L);
		break;
	case 0xA6: // AND A, [HL]
		opcode_AND_A_hl();
		break;
	case 0xA7: // AND A, A
		opcode_AND_A_r8(context.registers.A);
		break;
	case 0xA8: // XOR A, B
		opcode_XOR_A_r8(context.registers.B);
		break;
	case 0xA9: // XOR A, C
		opcode_XOR_A_r8(context.registers.C);
		break;
	case 0xAA: // XOR A, D
		opcode_XOR_A_r8(context.registers.D);
		break;
	case 0xAB: // XOR A, E
		opcode_XOR_A_r8(context.registers.E);
		break;
	case 0xAC: // XOR A, H
		opcode_XOR_A_r8(context.registers.H);
		break;
	case 0xAD: // XOR A, L
		opcode_XOR_A_r8(context.registers.L);
		break;
	case 0xAE: // XOR A, [HL]
		opcode_XOR_A_hl();
		break;
	case 0xAF: // XOR A, A
		opcode_XOR_A_r8(context.registers.A);
		break;
	case 0xB0: // OR A, B
		opcode_OR_A_r8(context.registers.B);
		break;
	case 0xB1: // OR A, C
		opcode_OR_A_r8(context.registers.C);
		break;
	case 0xB2: // OR A, D
		opcode_OR_A_r8(context.registers.D);
		break;
	case 0xB3: // OR A, E
		opcode_OR_A_r8(context.registers.E);
		break;
	case 0xB4: // OR A, H
		opcode_OR_A_r8(context.registers.H);
		break;
	case 0xB5: // OR A, L
		opcode_OR_A_r8(context.registers.L);
		break;
	case 0xB6: // OR A, [HL]
		opcode_OR_A_hl();
		break;
	case 0xB7: // OR A, A
		opcode_OR_A_r8(context.registers.A);
		break;
	case 0xB8: // CP A, B
		opcode_CP_A_r8(context.registers.B);
		break;
	case 0xB9: // CP A, C
		opcode_CP_A_r8(context.registers.C);
		break;
	case 0xBA: // CP A, D
		opcode_CP_A_r8(context.registers.D);
		break;
	case 0xBB: // CP A, E
		opcode_CP_A_r8(context.registers.E);
		break;
	case 0xBC: // CP A, H
		opcode_CP_A_r8(context.registers.H);
		break;
	case 0xBD: // CP A, L
		opcode_CP_A_r8(context.registers.L);
		break;
	case 0xBE: // CP A, [HL]
		opcode_CP_A_hl();
		break;
	case 0xBF: // CP A, A
		opcode_CP_A_r8(context.registers.A);
		break;
	case 0xC0: // RET NZ
		opcode_RET_cond(get_zero_flag() == 0);
		break;
	case 0xC1: // POP BC
		opcode_POP_r16(context.registers.BC);
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
		opcode_PUSH_r16(context.registers.BC);
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
		context.registers.PC += 2;
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
		opcode_POP_r16(context.registers.DE);
		break;
	case 0xD2: // JP NC, a16
		opcode_JP_cond(get_carry_flag() == 0);
		break;
	case 0xD3: // invalid
		std::cout << "Invalid opcode: " << std::hex << opcode << std::endl;
		context.registers.PC += 1;
		break;
	case 0xD4: // CALL NZ, a16
		opcode_CALL_cond(get_zero_flag() == 0);
		break;
	case 0xD5: // PUSH DE
		opcode_PUSH_r16(context.registers.DE);
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
		std::cout << "Invalid opcode: " << std::hex << opcode << std::endl;
		context.registers.PC += 1;
		break;
	case 0xDC: // CALL C, a16
		opcode_CALL_cond(get_carry_flag() == 1);
		break;
	case 0xDD: // invalid
		std::cout << "Invalid opcode: " << std::hex << opcode << std::endl;
		context.registers.PC += 1;
		break;
	case 0xDE: // SBC A, n8
		opcode_SBC_A_n8();
		break;
	case 0xDF: // RST $18
		opcode_RST(0x18);
		break;
	case 0xE0: // LDH [a8], A
		opcode_LDH_n8_r8(context.registers.A);
		break;
	case 0xE1: // POP HL
		opcode_POP_r16(context.registers.HL);
		break;
	case 0xE2: // LDH [C], A
		opcode_LDH_addr8_r8(context.registers.C, context.registers.A);
		break;
	case 0xE3: // invalid
	case 0xE4: // invalid
		std::cout << "Invalid opcode: " << std::hex << opcode << std::endl;
		context.registers.PC += 1;
		break;
	case 0xE5: // PUSH HL
		opcode_PUSH_r16(context.registers.HL);
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
		std::cout << "Invalid opcode: " << std::hex << opcode << std::endl;
		context.registers.PC += 1;
		break;
	case 0xEE: // XOR A, n8
		opcode_XOR_A_n8();
		break;
	case 0xEF: // RST $28
		opcode_RST(0x28);
		break;
	case 0xF0: // LDH A, [a8]
		opcode_LDH_r8_n8(context.registers.A);
		break;
	case 0xF1: // POP AF
		opcode_POP_r16(context.registers.AF);
		break;
	case 0xF2: // LDH A, [C]
		opcode_LDH_r8_addr8(context.registers.A, context.registers.C);
		break;
	case 0xF3: // DI
		opcode_DI();
		break;
	case 0xF4: // invalid
		std::cout << "Invalid opcode: " << std::hex << opcode << std::endl;
		context.registers.PC += 1;
		break;
	case 0xF5: // PUSH AF
		opcode_PUSH_r16(context.registers.AF);
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
		opcode_LD_r16_r16(context.registers.SP, context.registers.HL);
		break;
	case 0xFA: // LD A, [a16]
		opcode_LD_r8_n16(context.registers.A);
		break;
	case 0xFB: // EI
		opcode_EI();
		break;
	case 0xFC:
	case 0xFD: // invalid
		context.registers.PC++;
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