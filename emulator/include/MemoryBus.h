#ifndef EMULATOR_MEMORY_BUS_H_
#define EMULATOR_MEMORY_BUS_H_

#include <stdint.h>
#include <iostream>

namespace MemoryBus
{
	bool Initialize();
	void Shutdown();

	void Reset();

	void Write(uint16_t address, uint8_t data);
	uint8_t Read(uint16_t address);
}

#endif