#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "MemoryBus.h"
#include "CPU/CPU.h"
#include "Cartridge.h"
#include "Timer.h"
#include "PPU.h"

class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void Update(float dt);

	CPU* cpu = nullptr;
	PPU* ppu = nullptr;
	MemoryBus* mmu = nullptr;
	Cartridge* active_cartridge = nullptr;
	Timer* timer = nullptr;
	
	bool LoadROM(std::string rom_path);
private:

};

#endif