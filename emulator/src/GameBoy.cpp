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
		if (this->on_bootrom)
		{
			if (this->cpu->registers.PC == 0x100)
			{
				this->on_bootrom = false;
				this->active_cartridge->UnloadBootrom();
			}
		}

		// tick components
		this->cpu->Tick();

		this->timer->Update(1);
		this->ppu->Tick(1);
	}
}

uint8_t GameBoy::UpdateInput(uint8_t joyp)
{
	if (GET_BIT(joyp, 5) == 1 && GET_BIT(joyp, 4) == 1)
	{
		joyp |= 0x0F;
	}
	else
	{
		if (GET_BIT(joyp, 5) == 0)
		{
			for (int i = BUTTON_A; i <= BUTTON_START; i++)
			{
				if (keys[i])
				{
					if (GET_BIT(joyp, i - BUTTON_A))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, i - BUTTON_A);
					}
				}
				else
				{
					SET_BIT(joyp, i - BUTTON_A);
				}
			}
		}

		if (GET_BIT(joyp, 4) == 0)
		{
			for (int i = DPAD_RIGHT; i < BUTTON_START; i++)
			{
				if (keys[i])
				{
					if (GET_BIT(joyp, i))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, i);
					}
				}
				else
				{
					SET_BIT(joyp, i);
				}
			}
		}
	}

	return joyp;
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
		this->ppu->Reset();
		this->timer->Reset();

		this->on_bootrom = true;

		file.close();
		return true;
	}

	std::cerr << "Could not load ROM at path: " << rom_path << std::endl;
	return false;
}

void GameBoy::OnInputPressed(Joypad button)
{
	if (keys[button] == true)
	{
		return;
	}

	this->cpu->SetInterruptFlag(4, true);

	std::cout << "Button " << button << " pressed" << std::endl;
	keys[button] = true;
}

void GameBoy::OnInputReleased(Joypad button)
{
	if (keys[button] == false)
	{
		return;
	}

	std::cout << "Button " << button << " released" << std::endl;
	keys[button] = false;
}