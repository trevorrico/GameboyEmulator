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
		// update input
		uint8_t joyp = this->mmu->Read(0xFF00);
		if (GET_BIT(joyp, 5) == 1 && GET_BIT(joyp, 4) == 1)
		{
			joyp |= 0x0F;
		}
		else
		{
			if (GET_BIT(joyp, 5) == 0)
			{
				if (keys[BUTTON_A])
				{
					if (GET_BIT(joyp, 0))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 0);
					}
				}
				else
				{
					SET_BIT(joyp, 0);
				}

				if (keys[BUTTON_B])
				{
					if (GET_BIT(joyp, 1))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 1);
					}
				}
				else
				{
					SET_BIT(joyp, 1);
				}

				if (keys[BUTTON_SELECT])
				{
					if (GET_BIT(joyp, 2))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 2);
					}
				}
				else
				{
					SET_BIT(joyp, 2);
				}

				if (keys[BUTTON_START])
				{
					if (GET_BIT(joyp, 3))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 3);
					}
				}
				else
				{
					SET_BIT(joyp, 3);
				}
			}

			if (GET_BIT(joyp, 4) == 0)
			{
				if (keys[DPAD_RIGHT])
				{
					if (GET_BIT(joyp, 0))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 0);
					}
				}
				else
				{
					SET_BIT(joyp, 0);
				}

				if (keys[DPAD_LEFT])
				{
					if (GET_BIT(joyp, 1))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 1);
					}
				}
				else
				{
					SET_BIT(joyp, 1);
				}

				if (keys[DPAD_UP])
				{
					if (GET_BIT(joyp, 2))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 2);
					}
				}
				else
				{
					SET_BIT(joyp, 2);
				}

				if (keys[DPAD_DOWN])
				{
					if (GET_BIT(joyp, 3))
					{
						this->cpu->SetInterruptFlag(4, true);
						CLEAR_BIT(joyp, 3);
					}
				}
				else
				{
					SET_BIT(joyp, 3);
				}
			}
		}

		this->mmu->Write(0xFF00, joyp);

		// tick components
		uint32_t new_cycles = this->cpu->Step();
		if (new_cycles == 0)
		{
			new_cycles = 1;
		}

		this->timer->Update(new_cycles);
		this->ppu->Tick(new_cycles);
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