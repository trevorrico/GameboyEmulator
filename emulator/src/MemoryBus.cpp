#include "MemoryBus.h"
#include "GameBoy.h"

static uint8_t memory[0x10000]; // Soon this will be removed

MemoryBus::MemoryBus(GameBoy* gb)
{
	this->gb = gb;
	MemoryBus::Reset();
}

MemoryBus::~MemoryBus()
{
	
}

void MemoryBus::Reset()
{
	memset(memory, 0, 0x10000);
}

void MemoryBus::Write(uint32_t address, uint8_t data)
{
	if(address == 0xFF02 && data == 0x81)
	{
		std::cout << this->Read(0xFF01);
	}

	if (address == 0xFF46)
	{
		// TODO: Cycles for DMA transfer
		for (uint8_t i = 0; i <= 0x9F; i++)
		{
			this->gb->ppu->WriteOAM(0xFE00 + i, this->Read((data * 0x100) + i));
		}

		memory[0xFF46] = data;
		return;
	}
	
	if(gb->active_cartridge != nullptr)
	{
		if(address <= 0x7FFF)
		{
			gb->active_cartridge->WriteROM(address, data);
			return;
		}
		else if(address >= 0xA000 && address <= 0xBFFF)
		{
			gb->active_cartridge->WriteRAM(address, data);
			return;
		}
		else if(address >= 0x8000 && address <= 0x9FFF)
		{
			gb->ppu->WriteVRAM(address, data);
			return;
		}
		else if (address >= 0xFE00 && address <= 0xFE9F)
		{
			gb->ppu->WriteOAM(address, data);
			return;
		}
	}
	else
	{
		return;
	}

	memory[address] = data;
}

uint8_t MemoryBus::Read(uint32_t address)
{
	if(gb->active_cartridge != nullptr)
	{
		if(address <= 0x7FFF)
		{
			return gb->active_cartridge->ReadROM(address);
		}
		else if(address >= 0xA000 && address <= 0xBFFF)
		{
			return gb->active_cartridge->ReadRAM(address);
		}
		else if(address >= 0x8000 && address <= 0x9FFF)
		{
			return gb->ppu->ReadVRAM(address);
		}
		else if (address >= 0xFE00 && address <= 0xFE9F)
		{
			return gb->ppu->ReadOAM(address);
		}
	}
	else
	{
		return 0;
	}
	return memory[address];
}