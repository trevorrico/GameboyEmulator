#include "CPU/CPU.h"
#include "MemoryBus.h"
#include "GameBoy.h"

void CPU::opcode_LD_r8_n8(uint8_t& r8)
{
	r8 = this->gb->mmu->Read(this->registers.PC + 1);
	this->registers.PC += 2;
	this->cycles += 2;
}

void CPU::opcode_LD_r16(uint16_t& r16)
{
	uint8_t low = this->gb->mmu->Read(this->registers.PC + 1);
	uint8_t high = this->gb->mmu->Read(this->registers.PC + 2);
	r16 = (high << 8) | low;
	this->registers.PC += 3;
	this->cycles += 3;
}

void CPU::opcode_LD_r16_A(uint16_t& r16)
{
	this->gb->mmu->Write(r16, this->registers.A);
	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_LD_addr16_A()
{
	uint8_t low = this->gb->mmu->Read(this->registers.PC + 1);
	uint8_t high = this->gb->mmu->Read(this->registers.PC + 2);
	uint16_t r16 = (high << 8) | low;
	
	opcode_LD_r16_A(r16);

	this->registers.PC += 2;
	this->cycles += 2;
}

void CPU::opcode_LD_r8_r16address(uint8_t& r8, uint16_t r16address)
{
	r8 = this->gb->mmu->Read(r16address);
	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_LD_r8_n16(uint8_t& r8)
{
	uint8_t low = this->gb->mmu->Read(this->registers.PC + 1);
	uint8_t high = this->gb->mmu->Read(this->registers.PC + 2);

	uint16_t address = (high << 8) | low; 
	opcode_LD_r8_r16address(r8, address);

	this->registers.PC += 2;
	this->cycles += 2;
}

void CPU::opcode_LD_n16_r16(uint16_t& r16)
{
	uint8_t low = this->gb->mmu->Read(this->registers.PC + 1);
	uint8_t high = this->gb->mmu->Read(this->registers.PC + 2);

	uint16_t address = (high << 8) | low; 

	this->gb->mmu->Write(address, r16 & 0x00FF);
	this->gb->mmu->Write(address + 1, r16 >> 8);

	this->registers.PC += 3;
	this->cycles += 5;
}

void CPU::opcode_LD_r8_r8(uint8_t& r8_0, uint8_t& r8_1)
{
	r8_0 = r8_1;

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_LD_r16_r16(uint16_t& r8_0, uint16_t& r8_1)
{
	r8_0 = r8_1;

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_LD_hl_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	this->gb->mmu->Write(this->registers.HL, n8);

	this->registers.PC += 2;
	this->cycles += 3;
}

void CPU::opcode_LD_hl_sp_n8()
{
	int8_t e8 = static_cast<int8_t>(this->gb->mmu->Read(this->registers.PC + 1));
	this->registers.HL = this->registers.SP + e8;
	
	set_carry_flag((this->registers.SP & 0xFF) + (e8 & 0xFF) > 0xFF);
	set_half_carry_flag((this->registers.SP & 0x0F) + (e8 & 0x0F) > 0x0F);

	set_zero_flag(false);
	set_subtraction_flag(false);

	this->registers.PC += 2;
	this->cycles += 3;
}

void CPU::opcode_LD_hl_r8(uint8_t& r8)
{
	this->gb->mmu->Write(this->registers.HL, r8);

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_LDH_addr8_r8(uint8_t& addr, uint8_t& r8)
{
	this->gb->mmu->Write(0xFF00 + addr, r8);

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_LDH_r8_addr8(uint8_t& r8, uint8_t addr8)
{
	r8 = this->gb->mmu->Read(addr8 + 0xFF00);

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_LDH_n8_r8(uint8_t& r8)
{
	uint8_t addr = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_LDH_addr8_r8(addr, r8);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_LDH_r8_n8(uint8_t& r8)
{
	uint8_t addr = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_LDH_r8_addr8(r8, addr);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_INC_r16(uint16_t& r16)
{
	r16 += 1;
	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_INC_r8(uint8_t& r8)
{
	set_half_carry_flag((r8 & 0x0F) == 0x0F);

	r8 += 1;

	set_subtraction_flag(false);
	set_zero_flag(r8 == 0);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_INC_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);

	set_half_carry_flag((value & 0x0F) == 0x0F);

	value += 1;

	set_subtraction_flag(false);
	set_zero_flag(value == 0);

	this->gb->mmu->Write(this->registers.HL, value);

	this->registers.PC += 1;
	this->cycles += 3;
}

void CPU::opcode_DEC_r8(uint8_t& r8)
{
	set_half_carry_flag((r8 & 0x0F) == 0x00);

	r8 -= 1;

	set_subtraction_flag(true);
	set_zero_flag(r8 == 0);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_DEC_r16(uint16_t& r16)
{
	r16 -= 1;

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_DEC_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);

	set_half_carry_flag((value & 0x0F) == 0x00);

	value -= 1;

	set_subtraction_flag(true);
	set_zero_flag(value == 0);

	this->gb->mmu->Write(this->registers.HL, value);

	this->registers.PC += 1;
	this->cycles += 3;
}

void CPU::opcode_ADD_hl(uint16_t& r16)
{
	set_carry_flag(this->registers.HL + r16 > 0xFFFF);
	set_half_carry_flag((this->registers.HL & 0x0FFF) + (r16 & 0x0FFF) > 0x0FFF);
	set_subtraction_flag(false);

	this->registers.HL += r16;

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_ADD_A_r8(uint8_t& r8)
{
	set_carry_flag(this->registers.A + r8 > 0xFF);
	set_half_carry_flag((this->registers.A & 0x0F) + (r8 & 0x0F) > 0x0F);

	this->registers.A += r8;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_ADD_SP_e8()
{
	int8_t e8 = static_cast<int8_t>(this->gb->mmu->Read(this->registers.PC + 1));
	
	int32_t result = this->registers.SP + e8;

	set_carry_flag((this->registers.SP & 0xFF) + (e8 & 0xFF) > 0xFF);
	set_half_carry_flag((this->registers.SP & 0x0F) + (e8 & 0x0F) > 0x0F);

	this->registers.SP = static_cast<uint16_t>(result);

	set_zero_flag(false);
	set_subtraction_flag(false);

	this->registers.PC += 2;
	this->cycles += 4;
}

void CPU::opcode_ADD_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_ADD_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_ADD_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_ADD_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_ADC_A_r8(uint8_t& r8)
{
	uint8_t carry = get_carry_flag();
	
	set_carry_flag(this->registers.A + r8 + carry > 0xFF);
	set_half_carry_flag((this->registers.A & 0x0F) + (r8 & 0x0F) + carry > 0x0F);

	this->registers.A += r8 + carry;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_ADC_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_ADC_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_ADC_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_ADC_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_SUB_A_r8(uint8_t& r8)
{
	set_carry_flag(this->registers.A < r8);
	set_half_carry_flag((this->registers.A & 0x0F) - (r8 & 0x0F) < 0);

	this->registers.A -= r8;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(true);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_SUB_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_SUB_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_SUB_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_SUB_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_SBC_A_r8(uint8_t& r8)
{
	uint8_t carry = get_carry_flag();
	uint8_t val = r8 + carry;

	set_carry_flag(this->registers.A < r8 + carry);
	set_half_carry_flag((this->registers.A & 0x0F) - (r8 & 0x0F) - carry < 0);

	this->registers.A -= val;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(true);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_SBC_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_SBC_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_SBC_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_SBC_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_AND_A_r8(uint8_t& r8)
{
	this->registers.A &= r8;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(true);
	set_carry_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_AND_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_AND_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_AND_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_AND_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_XOR_A_r8(uint8_t& r8)
{
	this->registers.A ^= r8;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_XOR_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_XOR_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_XOR_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_XOR_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_OR_A_r8(uint8_t& r8)
{
	this->registers.A |= r8;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_OR_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_OR_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_OR_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_OR_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_CP_A_r8(uint8_t& r8)
{
	uint8_t result = this->registers.A - r8;

	set_zero_flag(result == 0);
	set_subtraction_flag(true);
	set_half_carry_flag((this->registers.A & 0x0F) < (r8 & 0x0F));
	set_carry_flag(this->registers.A < r8);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_CP_A_n8()
{
	uint8_t n8 = this->gb->mmu->Read(this->registers.PC + 1);
	opcode_CP_A_r8(n8);
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_CP_A_hl()
{
	uint8_t value = this->gb->mmu->Read(this->registers.HL);
	opcode_CP_A_r8(value);
	this->cycles += 1;
}

void CPU::opcode_RLCA()
{
	uint8_t carry = (this->registers.A & 0x80) >> 7;
	this->registers.A = (this->registers.A << 1) | carry;

	set_zero_flag(0);
	set_half_carry_flag(0);
	set_subtraction_flag(0);
	set_carry_flag(carry);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_RRCA()
{
	uint8_t carry = (this->registers.A & 0x01);
	this->registers.A = (this->registers.A >> 1) | (carry << 7);

	set_zero_flag(0);
	set_half_carry_flag(0);
	set_subtraction_flag(0);
	set_carry_flag(carry);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_RLA()
{
	uint8_t carry = get_carry_flag();
	set_carry_flag((this->registers.A & 0x80) >> 7);
	set_zero_flag(false);
	set_subtraction_flag(false);
	set_half_carry_flag(false);

	this->registers.A = (this->registers.A << 1) | carry;

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_RRA()
{
	uint8_t carry = get_carry_flag();
	set_carry_flag((this->registers.A & 0x01));
	set_zero_flag(false);
	set_subtraction_flag(false);
	set_half_carry_flag(false);

	this->registers.A = (this->registers.A >> 1) | (carry << 7);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_JR()
{
	int8_t offset = static_cast<int8_t>(this->gb->mmu->Read(this->registers.PC + 1));

	this->registers.PC += 2 + offset;
	this->cycles += 3;
}

void CPU::opcode_JR_condition(bool condition)
{
	if(condition)
	{
		opcode_JR();
	}
	else
	{
		this->registers.PC += 2;
		this->cycles += 2;
	}
}

void CPU::opcode_DAA()
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

		this->registers.A -= adjustment;
	}
	else
	{
		if(get_half_carry_flag() || (this->registers.A & 0x0F) > 0x09)
		{
			adjustment += 0x06;
		}

		if(get_carry_flag() || this->registers.A > 0x99)
		{
			adjustment += 0x60;
			set_carry_flag(true);
		}

		this->registers.A += adjustment;
	}

	set_zero_flag(this->registers.A == 0);
	set_half_carry_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_CPL()
{
	this->registers.A = ~this->registers.A;

	set_subtraction_flag(true);
	set_half_carry_flag(true);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_SCF()
{
	set_carry_flag(true);
	set_half_carry_flag(false);
	set_subtraction_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_CCF()
{
	set_carry_flag(!get_carry_flag());
	set_half_carry_flag(false);
	set_subtraction_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_POP_r16(uint16_t& r16)
{
	uint8_t low = this->gb->mmu->Read(this->registers.SP);
	this->registers.SP++;
	uint8_t high = this->gb->mmu->Read(this->registers.SP);
	this->registers.SP++;

	r16 = (high << 8) | low;

	this->registers.PC += 1;
	this->cycles += 3;
}

void CPU::opcode_POP_AF()
{
	opcode_POP_r16(this->registers.AF);
	this->registers.F &= 0xF0; // lower 4 bits of F are always 0

	uint8_t low = this->registers.AF & 0x00FF;

	set_zero_flag(low >> 7);
	set_subtraction_flag((low >> 6) & 0x01);
	set_half_carry_flag((low >> 5) & 0x01);
	set_carry_flag((low >> 4) & 0x01);
}

void CPU::opcode_PUSH_r16(uint16_t& r16)
{
	this->PushAddress(r16);

	this->registers.PC += 1;
	this->cycles += 4;
}

void CPU::opcode_RET()
{
	opcode_POP_r16(this->registers.PC);
	this->registers.PC -= 1;
	this->cycles += 1;
}

void CPU::opcode_RET_cond(uint8_t cond)
{
	if(cond)
	{
		opcode_RET();
		this->cycles += 1;
	}
	else
	{
		this->registers.PC++;
		this->cycles += 2;
	}
}

void CPU::opcode_JP_r16(uint16_t& r16)
{
	this->registers.PC = r16;
	this->cycles += 4;
}

void CPU::opcode_JP_n16()
{
	uint8_t low = this->gb->mmu->Read(this->registers.PC + 1);
	uint8_t high = this->gb->mmu->Read(this->registers.PC + 2);

	uint16_t value = (high << 8) | low;
	opcode_JP_r16(value);
}

void CPU::opcode_JP_cond(uint8_t cond)
{
	if(cond)
	{
		opcode_JP_n16();
	}
	else
	{
		this->registers.PC += 3;
		this->cycles += 4;
	}
}

void CPU::opcode_JP_hl()
{
	this->registers.PC = this->registers.HL;
	this->cycles += 1;
}

void CPU::opcode_CALL()
{
	uint8_t low = this->gb->mmu->Read(this->registers.PC + 1);
	uint8_t high = this->gb->mmu->Read(this->registers.PC + 2);
	this->registers.PC += 3;

	uint16_t value = (high << 8) | low;

	this->PushAddress(this->registers.PC);
	this->registers.PC = value;
	this->cycles += 6;
}

void CPU::opcode_CALL_cond(uint8_t cond)
{
	if(cond)
	{
		opcode_CALL();
	}
	else
	{
		this->registers.PC += 3;
		this->cycles += 3;
	}
}

void CPU::opcode_RST(uint16_t address)
{
	this->PushAddress(this->registers.PC + 1);
	this->registers.PC = address;
	this->cycles += 4;
}

void CPU::opcode_EI()
{
	this->IME = true;

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_DI()
{
	this->IME = false;

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_RETI()
{
	opcode_EI();
	opcode_RET();

	this->cycles -=1;
}

void CPU::opcode_HALT()
{
	// WIP
	this->halted = true;
	this->registers.PC += 1;
}

// CB
void CPU::opcode_RLC(uint8_t& r8)
{
	uint8_t carry = (r8 & 0x80) >> 7;
	r8 = (r8 << 1) | carry;

	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(carry);

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_RLC_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_RLC(val);
	this->gb->mmu->Write(this->registers.HL, val);
	
	this->cycles += 2;
}

void CPU::opcode_RRC(uint8_t& r8)
{
	uint8_t carry = (r8 & 0x01);
	r8 = (r8 >> 1) | (carry << 7);

	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(carry);

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_RRC_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_RRC(val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_RL(uint8_t& r8)
{
	uint8_t bit = (r8 & 0x80) >> 7;

	uint8_t carry = get_carry_flag();

	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(bit);

	r8 = (r8 << 1) | carry;

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_RL_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_RL(val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_RR(uint8_t& r8)
{
	uint8_t bit = (r8 & 0x01);

	uint8_t carry = get_carry_flag();

	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(bit);

	r8 = (r8 >> 1) | (carry << 7);

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_RR_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_RR(val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_SLA(uint8_t& r8)
{
	uint8_t bit = (r8 & 0x80) >> 7;

	uint8_t carry = get_carry_flag();
	
	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(bit);

	r8 << 1;

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_SLA_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_SLA(val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_SRA(uint8_t& r8)
{
	uint8_t bit = (r8 & 0x01);
	uint8_t high_bit = (r8 & 0x80);

	uint8_t carry = get_carry_flag();
	
	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(bit);

	r8 = (r8 >> 1) | high_bit;

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_SRA_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_SRA(val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_SWAP(uint8_t& r8)
{
	uint8_t upper = (r8 & 0xF0) >> 4;
	uint8_t lower = (r8 & 0x0F) << 4;
	
	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(false);

	r8 = upper | lower;

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_SWAP_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_SWAP(val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_SRL(uint8_t& r8)
{
	uint8_t bit = (r8 & 0x01);

	uint8_t carry = get_carry_flag();
	
	set_zero_flag(r8 == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);
	set_carry_flag(bit);

	r8 = (r8 >> 1);

	this->cycles += 2;
	this->registers.PC += 1;
}

void CPU::opcode_SRL_hl()
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_SRL(val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_BIT(uint8_t u3, uint8_t r8)
{
	uint8_t bit = (r8 >> u3) & 0x01;

	set_zero_flag(bit == 0);
	set_subtraction_flag(false);
	set_half_carry_flag(false);

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_BIT_hl(uint8_t u3)
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_BIT(u3, val);

	this->cycles += 1;
}

void CPU::opcode_RES(uint8_t u3, uint8_t& r8)
{
	r8 &= 0xFE << u3;

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_RES_hl(uint8_t u3)
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_RES(u3, val);
	this->gb->mmu->Write(this->registers.HL, val);

	this->cycles += 2;
}

void CPU::opcode_SET(uint8_t u3, uint8_t& r8)
{
	r8 |= 0x01 << u3;

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_SET_hl(uint8_t u3)
{
	uint8_t val = this->gb->mmu->Read(this->registers.HL);
	opcode_SET(u3, val);
	this->gb->mmu->Write(this->registers.HL, val);
	
	this->cycles += 2;
}