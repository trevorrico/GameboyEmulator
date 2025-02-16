#include "CPU/CPU.h"
#include "MemoryBus.h"
#include "GameBoy.h"

CPU::CPU(GameBoy* gb) {
	this->gb = gb;
	CPU::Reset();
}

CPU::~CPU()
{
}

void CPU::Reset()
{
	this->registers.AF = 0x01B0;
	this->registers.BC = 0x0013;
	this->registers.DE = 0x00D8;
	this->registers.HL = 0x014D;
	this->registers.SP = 0xFFFE;
	this->registers.PC = 0x0100;
	this->cycles = 0;

	this->IME = false;
	this->halted = false;
}

bool CPU::ProcessInterrupt(uint8_t interrupts_fired, uint8_t bit, uint8_t address)
{
	if(interrupts_fired & (0x01 << bit))
	{
		this->cycles += 5;

		uint8_t IF = this->gb->mmu->Read(0xFF0F);
		this->gb->mmu->Write(0xFF0F, IF & (0xFE << bit));
		this->registers.PC = address;
		IME = false;
		return true;
	}

	return false;
}

void CPU::HandleInterrupts()
{
	uint8_t IE = this->gb->mmu->Read(0xFFFF);
	uint8_t IF = this->gb->mmu->Read(0xFF0F);

	uint8_t interrupts_fired = IF & IE;
	if(interrupts_fired == 0x00)
	{
		return;
	}

	if(halted)
	{
		// TODO: Halt bug
		halted = false;
	}

	if(IME == false)
	{
		return;
	}

	PushAddress(this->registers.PC);

	if(ProcessInterrupt(interrupts_fired, 0, 0x40)) {return;} // VBlank interrupt
	if(ProcessInterrupt(interrupts_fired, 1, 0x48)) {return;} // STAT interrupt
	if(ProcessInterrupt(interrupts_fired, 2, 0x50)) {return;} // Timer interrupt
	if(ProcessInterrupt(interrupts_fired, 3, 0x58)) {return;} // Serial interrupt
	if(ProcessInterrupt(interrupts_fired, 4, 0x60)) {return;} // Joypad interrupt
}

void CPU::PushAddress(uint16_t address)
{
	this->registers.SP--;
	this->gb->mmu->Write(this->registers.SP, address >> 8);
	this->registers.SP--;
	this->gb->mmu->Write(this->registers.SP, (uint8_t)(address & 0x00FF));
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

uint32_t CPU::Step()
{
	uint32_t start_cycles = this->cycles;

	HandleInterrupts();

	if(this->halted)
	{
		uint32_t diff = this->cycles - start_cycles;
		this->cycles %= CLOCK_SPEED;
		return diff;
	}

	uint8_t opcode = this->gb->mmu->Read(this->registers.PC);

	/* Logging, remove comment to activate
	if(this->log_lines < 1258895)
	{
		log_string << "A: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.A);
		log_string << " F: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.F);
		log_string << " B: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.B);
		log_string << " C: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.C);
		log_string << " D: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.D);
		log_string << " E: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.E);
		log_string << " H: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.H);
		log_string << " L: " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.L);
		log_string << " SP: " << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.SP);
		log_string << " PC: 00:" << std::setw(4) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->registers.PC);
		log_string << " (" << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->gb->mmu->Read(this->registers.PC));
		log_string << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->gb->mmu->Read(this->registers.PC + 1));
		log_string << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->gb->mmu->Read(this->registers.PC + 2));
		log_string << " " << std::setw(2) << std::setfill('0') << std::uppercase << std::hex << uintptr_t(this->gb->mmu->Read(this->registers.PC + 3)) << ")" << std::endl;

		this->log_lines++;
		if(this->log_lines == 1258895)
		{
			std::ofstream out;
			out.open("log.txt", std::ofstream::out | std::ofstream::trunc);

			out << this->log_string.rdbuf();

			out.close();

			std::cout << "Created log file!" << std::endl;
		}
	}
	else
	{
		this->cycles += 1;
		return;
	}
	*/

	if(opcode == 0xCB)
	{
		this->registers.PC++;
		opcode = this->gb->mmu->Read(this->registers.PC);
		this->ProcessCBOpcode(opcode);
	}
	else
	{
		this->ProcessOpcode(opcode);
	}

	uint32_t diff = this->cycles - start_cycles;
	this->cycles %= CLOCK_SPEED;
	return diff;
}