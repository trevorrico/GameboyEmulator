#include "GameBoy.h"

GameBoy::GameBoy()
{
	this->mmu = new MemoryBus(this);
	this->cpu = new CPU(this);
	this->ppu = new PPU(this);
	this->timer = new Timer(this);
	this->active_cartridge = nullptr;
}

GameBoy::~GameBoy()
{
	if(this->active_cartridge)
	{
		delete this->active_cartridge;
	}

	delete this->timer;
	delete this->ppu;
	delete this->cpu;
	delete this->mmu;
}

void GameBoy::Update(float dt)
{
	if(this->active_cartridge)
	{
		uint32_t new_cycles = this->cpu->Step();

		this->timer->Update(new_cycles);
	}
}

bool GameBoy::LoadROM(std::string rom_path)
{
	std::ifstream file(rom_path, std::ios::binary);
	if(file.is_open())
	{
		std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
		
		if(this->active_cartridge)
		{
			delete this->active_cartridge;
		}
		
		this->mmu->Reset();

		this->active_cartridge = new Cartridge();
		this->active_cartridge->LoadROM(rom_path, buffer.data(), buffer.size());

		this->cpu->Reset();

		file.close();
		return true;
	}

	std::cerr << "Could not load ROM at path: " << rom_path << std::endl;
	return false;
}