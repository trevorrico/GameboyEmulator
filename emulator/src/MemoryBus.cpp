#include "MemoryBus.h"

static uint8_t memory[0x10000];

bool MemoryBus::Initialize()
{
	MemoryBus::Reset();
	return true;
}

void MemoryBus::Shutdown()
{

}

void MemoryBus::Reset()
{
	memset(memory, 0, 0x10000);
}

void MemoryBus::Write(uint16_t address, uint8_t data)
{
	memory[address] = data;
}

uint8_t MemoryBus::Read(uint16_t address)
{
	return memory[address];
}