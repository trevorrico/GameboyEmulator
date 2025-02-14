#include "GameBoy.h"

GameBoy::GameBoy()
{
	this->mmu = new MemoryBus(this);
	this->cpu = new CPU(this);
	this->active_cartridge = nullptr;
}

GameBoy::~GameBoy()
{
	if(this->active_cartridge)
	{
		delete this->active_cartridge;
	}

	delete this->cpu;
	delete this->mmu;
}

void GameBoy::Update(float dt)
{
	if(this->active_cartridge)
	{
		uint64_t cycleCount = 4194304.0f * dt;
		if(cycleCount < 1)
		{
			cycleCount = 1;
		}
		else if(cycleCount > 41943) // 0.01 seconds
		{
			cycleCount = 41943;
		}

		cycleCount = 1;

		uint32_t currentCount = this->cpu->cycles;
		while(this->cpu->cycles < currentCount + cycleCount)
		{
			this->cpu->Step();
		}

		this->cpu->cycles %= 4194304;
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