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

enum Joypad
{
	DPAD_RIGHT = 0,
	DPAD_LEFT = 1,
	DPAD_UP = 2,
	DPAD_DOWN = 3,
	BUTTON_A = 4,
	BUTTON_B = 5,
	BUTTON_SELECT = 6,
	BUTTON_START = 7
};

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
	
	void OnInputPressed(Joypad button);
	void OnInputReleased(Joypad button);

	uint8_t UpdateInput(uint8_t joyp);

	bool LoadROM(std::string rom_path);
private:
	bool keys[8];
	bool on_bootrom = false;
};

#endif