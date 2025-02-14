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
	this->gb->mmu->Write(this->registers.HL, this->registers.SP + e8);
	
	set_carry_flag((this->registers.SP & 0xFF) + e8 > 0xFF);
	set_half_carry_flag((this->registers.SP & 0x0F) + e8 > 0x0F);

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
	this->gb->mmu->Write(r8 + 0xFF00, r8);

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
	set_half_carry_flag(this->registers.HL & 0x0FFF + r16 > 0x0FFF);
	set_zero_flag(false);

	this->registers.HL += r16;

	this->registers.PC += 1;
	this->cycles += 2;
}

void CPU::opcode_ADD_A_r8(uint8_t& r8)
{
	set_carry_flag(this->registers.A + r8 > 0xFF);
	set_half_carry_flag((this->registers.A & 0x0F) + r8 > 0x0F);

	this->registers.A += r8;

	set_zero_flag(this->registers.A == 0);
	set_subtraction_flag(false);

	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_ADD_SP_e8()
{
	int8_t e8 = static_cast<int8_t>(this->gb->mmu->Read(this->registers.PC + 1));
	
	set_carry_flag((this->registers.SP & 0xFF) + e8 > 0xFF);
	set_half_carry_flag((this->registers.SP & 0x0F) + e8 > 0x0F);

	this->registers.SP += e8;

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
	uint8_t val = r8 + carry;

	opcode_ADD_A_r8(val);
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
	set_half_carry_flag((this->registers.A & 0x0F) < r8 & 0x0F);

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

	opcode_SUB_A_r8(val);
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
		if(get_half_carry_flag() || this->registers.A & 0x0F > 0x09)
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

void CPU::opcode_PUSH_r16(uint16_t& r16)
{
	this->registers.SP--;
	this->gb->mmu->Write(this->registers.SP, r16 >> 8);
	this->registers.SP--;
	this->gb->mmu->Write(this->registers.SP, (uint8_t)(r16 & 0x00FF));

	this->registers.PC += 1;
	this->cycles += 4;
}

void CPU::opcode_RET()
{
	opcode_POP_r16(this->registers.PC);
	this->registers.PC -= 1; // I do not know if this should be here 
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

	opcode_PUSH_r16(this->registers.PC);
	this->registers.PC = value;
	this->cycles += 2;
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
	opcode_PUSH_r16(address);
	opcode_JP_r16(address);

	this->cycles -= 4;
}

void CPU::opcode_EI()
{
	// TODO
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_DI()
{
	// TODO
	this->registers.PC += 1;
	this->cycles += 1;
}

void CPU::opcode_RETI()
{
	// TODO
	opcode_EI();
	opcode_RET();
}