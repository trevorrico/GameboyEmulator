#include "CPU/CPU.h"
#include "MemoryBus.h"

CPU::CPU(GameBoy* gb) {
	this->gb = gb;
	CPU::Reset();
}

CPU::~CPU()
{
}

void CPU::Reset()
{
	this->registers.AF = 0x1180;
	this->registers.BC = 0x0000;
	this->registers.DE = 0xFF56;
	this->registers.HL = 0x000D;
	this->registers.SP = 0xFFFE;
	this->registers.PC = 0x0100;
	this->cycles = 0;
}

void CPU::set_zero_flag(bool set) // Z
{
	if(set)
	{
		this->registers.F |= (0x01 << 7);
	}
	else
	{
		this->registers.F &= ~(0x01 << 7);
	}
}

void CPU::set_carry_flag(bool set) // C
{
	if(set)
	{
		this->registers.F |= (0x01 << 4);
	}
	else
	{
		this->registers.F &= ~(0x01 << 4);
	}
}

void CPU::set_subtraction_flag(bool set) // N
{
	if(set)
	{
		this->registers.F |= (0x01 << 6);
	}
	else
	{
		this->registers.F &= ~(0x01 << 6);
	}
}

void CPU::set_half_carry_flag(bool set) // H
{
	if(set)
	{
		this->registers.F |= (0x01 << 5);
	}
	else
	{
		this->registers.F &= ~(0x01 << 5);
	}
}

uint8_t CPU::get_carry_flag()
{
	return (this->registers.F >> 4) & 0x01;
}

uint8_t CPU::get_zero_flag()
{
	return (this->registers.F >> 7);
}

uint8_t CPU::get_subtraction_flag()
{
	return (this->registers.F >> 6) & 0x01;
}

uint8_t CPU::get_half_carry_flag()
{
	return (this->registers.F >> 5) & 0x01;
}