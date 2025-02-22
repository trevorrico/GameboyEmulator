#ifndef EMULATOR_MEMORY_BUS_H_
#define EMULATOR_MEMORY_BUS_H_

#include <stdint.h>
#include <iostream>
#include <cstring>

class GameBoy;

class MemoryBus
{
public:
	MemoryBus(GameBoy* gameboy);
	~MemoryBus();

	void Reset();

	void Write(uint32_t address, uint8_t data);
	uint8_t Read(uint32_t address);
private:
	GameBoy* gb;
};

#endif